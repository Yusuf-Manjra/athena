# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from difflib import unified_diff
from pathlib import Path
import subprocess

from .Helpers import warnings_count
from .Inputs import references_CVMFS_path, references_EOS_path
from .References import references_map
from .Test import TestSetup, WorkflowCheck, WorkflowTest, WorkflowType


class FailedOrPassedCheck(WorkflowCheck):
    """Was the q test successful? To check simply count the number of lines containing the string "successful run"."""

    def run(self, test: WorkflowTest) -> bool:
        result = True
        for step in test.steps:
            self.logger.info("-----------------------------------------------------")
            log = test.validation_path / f"log.{step}"
            counter = 0
            warnings = []
            errors = []
            if log.exists():
                with log.open() as file:
                    for line in file:
                        if ("ERROR" in line and "| ERROR |" not in line) or ("FATAL" in line and "| FATAL |" not in line):
                            errors.append(line[9:].strip())
                        elif "WARNING" in line and "| WARNING |" not in line:
                            warnings.append(line[9:].strip())
                        elif '"successful run"' in line:
                            counter += 1

            if warnings:
                self.logger.info(f"{step} validation test step WARNINGS")
                warnings = list(dict.fromkeys(warnings))
                for w in warnings:
                    self.logger.info(f"  {w}")
                self.logger.info("-----------------------------------------------------")

            if errors:

                self.logger.info(f"{step} validation test step ERRORS")
                errors = list(dict.fromkeys(errors))
                for e in errors:
                    self.logger.info(f"  {e}")
                self.logger.info("-----------------------------------------------------")

            if counter:
                self.logger.info(f"{step} validation test step successful")
            else:
                result = False
                if log.exists():
                    self.logger.error(f"{step} validation test step failed")
                    self.logger.error(f"Full {step} step log:")
                    with log.open() as file:
                        for line in file:
                            print(f"  {line.strip()}")
                    self.logger.info("-----------------------------------------------------")
                else:
                    self.logger.error(f"{step} validation test step did not run")

            if self.setup.validation_only:
                continue  # Skip checking reference test because in this mode the clean tests have not been run

            log = test.reference_path / f"log.{step}"
            counter = 0
            with log.open() as file:
                for line in file:
                    if '"successful run"' in line:
                        counter += 1

            if counter:
                self.logger.info(f"{step} reference test step successful")
            else:
                self.logger.error(f"{step} reference test step failed")
                result = False

        if result:
            self.logger.info(f"All {test.ID} athena steps completed successfully\n")
        else :
            self.logger.error(f"One or more {test.ID} Athena steps failed. Please investigate the cause.\n")

        return result


class FrozenTier0PolicyCheck(WorkflowCheck):
    """Run Frozen Tier0 Policy Check."""

    def __init__(self, setup: TestSetup, input_format: str, max_events: int) -> None:
        super().__init__(setup)
        self.format = input_format
        self.max_events = str(max_events)

    def run(self, test: WorkflowTest) -> bool:
        self.logger.info("---------------------------------------------------------------------------------------")
        self.logger.info(f"Running {test.ID} Frozen Tier0 Policy Check on {self.format} for {self.max_events} events")

        reference_path: Path = test.reference_path
        diff_rules_file: Path = self.setup.diff_rules_path

        # Read references from EOS/CVMFS
        if self.setup.validation_only:
            # Resolve the subfolder first. Results are stored like: main_folder/q-test/branch/version/.
            # This should work both in standalone and CI
            # Use EOS if mounted, otherwise CVMFS
            reference_revision = references_map[f"{test.ID}"]
            eos_path = Path(references_EOS_path)
            reference_path = eos_path / self.setup.release_ID / test.ID / reference_revision
            diff_rules_file = eos_path / self.setup.release_ID / test.ID
            if reference_path.exists():
                self.logger.info("EOS is mounted, going to read the reference files from there instead of CVMFS")
            else:
                self.logger.info("EOS is not mounted, going to read the reference files from CVMFS")
                cvmfs_path = Path(references_CVMFS_path)
                reference_path = cvmfs_path / self.setup.release_ID / test.ID / reference_revision
                diff_rules_file = cvmfs_path / self.setup.release_ID / test.ID

        diff_rules_file /= f"{self.format}_diff-exclusion-list.txt"

        self.logger.info(f"Reading the reference file from location {reference_path}")

        if diff_rules_file.exists():
            self.logger.info(f"Reading the diff rules file from location {diff_rules_file}")
            exclusion_list = []
            with diff_rules_file.open() as f:
                for line in f:
                    exclusion_list.append(r"'{}'".format(line.rstrip()))
        else:
            self.logger.info("No diff rules file exists, using the default list")
            exclusion_list = [r"'index_ref'", r"'(.*)_timings\.(.*)'", r"'(.*)_mems\.(.*)'", r"'(.*)TrigCostContainer(.*)'"]

        file_name = f"my{self.format}.pool.root"
        reference_file = reference_path / file_name
        validation_file = test.validation_path / file_name
        log_file = test.validation_path / f"diff-root-{test.ID}.{self.format}.log"
        exclusion_list = " ".join(exclusion_list)

        comparison_command = f"acmd.py diff-root {reference_file} {validation_file} --nan-equal --error-mode resilient --ignore-leaves {exclusion_list} --entries {self.max_events} > {log_file} 2>&1"
        output, error = subprocess.Popen(["/bin/bash", "-c", comparison_command], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
        output, error = output.decode("utf-8"), error.decode("utf-8")

        # We want to catch/print both container additions/subtractions as well as
        # changes in these containers.  `allGood_return_code` is meant to catch
        # other issues found in the diff (not expected, but just to be safe)
        passed_frozen_tier0_test = True
        all_good = False
        with log_file.open() as file:
            for line in file:
                if "WARNING" in line:  # Catches container addition/subtractions
                    self.logger.error(line.strip())
                    passed_frozen_tier0_test = False
                if "leaves differ" in line:  # Catches changes in branches
                    self.logger.error(line.strip())
                    passed_frozen_tier0_test = False
                if "INFO all good." in line:
                    all_good = True

        result = passed_frozen_tier0_test and all_good
        if result:
            self.logger.info("Passed!\n")
        else:
            # print CI helper directly to avoid logger decorations
            print(f"ATLAS-CI-ADD-LABEL: {test.run.value}-{test.type.value}-output-changed")
            print()

            self.logger.error(f"Your change breaks the frozen tier0 policy in test {test.ID}:")
            with log_file.open() as file:
                for line in file:
                    self.logger.info(f"  {line.strip()}")
                self.logger.info("-----------------------------------------------------\n")

        return result


class AODContentCheck(WorkflowCheck):
    """Run AOD Content Check."""

    def run(self, test: WorkflowTest) -> bool:
        self.logger.info("---------------------------------------------------------------------------------------")
        self.logger.info(f"Running {test.ID} AOD content check")

        file_name = "myAOD.pool.root"
        output_name = "AOD_content.txt"

        validation_file = test.validation_path / file_name
        validation_output = test.validation_path / output_name
        validation_command = f"acmd.py chk-file {validation_file} | awk '/---/{{flag=1;next}}/===/{{flag=0}}flag' | awk '{{print $10}}' | sort > {validation_output}"

        output_val, error_val = subprocess.Popen(["/bin/bash", "-c", validation_command], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
        output_val, error_val = output_val.decode("utf-8"), error_val.decode("utf-8")
        if error_val:
            self.logger.error(f"Something went wrong with retrieving the content for test {test.ID}:")
            self.logger.error(error_val)

        # Read references
        if self.setup.validation_only:
            # try to get the reference
            reference_path = test.validation_path
            reference_output_name = f"{self.setup.release_ID}_{test.ID}_AOD_content.ref"
            reference_output = reference_path / reference_output_name
            subprocess.Popen(["/bin/bash", "-c", f"cd {reference_path}; get_files -data {reference_output_name}"], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
            if not reference_output.exists():
                self.logger.info(f"No reference file '{reference_output_name}' to compare the content with.")
                return True
        else:
            reference_path = test.reference_path
            reference_output = reference_path / output_name
            reference_file = reference_path / file_name

            reference_command = f"acmd.py chk-file {reference_file} | awk '/---/{{flag=1;next}}/===/{{flag=0}}flag' | awk '{{print $10}}' | sort > {reference_output}"
            subprocess.Popen(["/bin/bash", "-c", reference_command], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()

        # Compute the diff
        with reference_output.open() as f:
            reference_lines = sorted(f.readlines())
        with validation_output.open() as f:
            validation_lines = sorted(f.readlines())

        # Remove HLT containers in some cases
        if test.type == WorkflowType.MCReco or test.type == WorkflowType.MCPileUpReco:
            reference_lines = [line for line in reference_lines if not (line.startswith('HLT') or line.startswith('L1') or line.startswith('LVL1'))]
            validation_lines = [line for line in validation_lines if not (line.startswith('HLT') or line.startswith('L1') or line.startswith('LVL1'))]

        result = list(unified_diff(reference_lines, validation_lines, fromfile="reference", tofile="validation"))
        if not result:
            self.logger.info("Passed!\n")
        else:
            # print CI helper directly to avoid logger decorations
            print(f"ATLAS-CI-ADD-LABEL: {test.run.value}-{test.type.value}-output-changed")
            print()

            self.logger.error(f"Your change breaks the frozen tier0 policy in test {test.ID}:")
            for line in result:
                self.logger.info(f"  {line.strip()}")
            self.logger.info("-----------------------------------------------------\n")

        return not result


class AODDigestCheck(WorkflowCheck):
    """Run AOD Digest Check."""

    def __init__(self, setup: TestSetup, max_events: int = -1) -> None:
        super().__init__(setup)
        self.max_events = str(max_events)

    def run(self, test: WorkflowTest) -> bool:
        self.logger.info("---------------------------------------------------------------------------------------")
        self.logger.info(f"Running {test.ID} AOD digest")

        file_name = "myAOD.pool.root"
        output_name = "AOD_digest.txt"

        validation_file = test.validation_path / file_name
        validation_output = test.validation_path / output_name
        validation_log_file = test.validation_path / f"AODdigest-{test.ID}.log"
        validation_command = f"xAODDigest.py {validation_file} {validation_output} > {validation_log_file} 2>&1"

        output_val, error_val = subprocess.Popen(["/bin/bash", "-c", validation_command], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
        output_val, error_val = output_val.decode("utf-8"), error_val.decode("utf-8")
        if error_val:
            self.logger.error(f"Something went wrong with the digest calculation for test {test.ID}:")
            self.logger.error(error_val)

        # Read references
        if self.setup.validation_only:
            # try to get the reference
            reference_path = test.validation_path
            reference_output_name = f"{self.setup.release_ID}_{test.ID}_AOD_digest.ref"
            reference_output = reference_path / reference_output_name
            subprocess.Popen(["/bin/bash", "-c", f"cd {reference_path}; get_files -data {reference_output_name}"], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
            if not reference_output.exists():
                self.logger.info(f"No reference file '{reference_output_name}' to compare the digest with. Printing the full digest:")
                with validation_output.open() as f:
                    for line in f:
                        print(f"    {line.strip()}")
                return True
        else:
            reference_path = test.reference_path
            reference_output = reference_path / output_name
            reference_file = reference_path / file_name
            reference_log_file = test.reference_path / f"AODdigest-{test.ID}.log"

            reference_command = f"xAODDigest.py {reference_file} {reference_output} > {reference_log_file} 2>&1"
            subprocess.Popen(["/bin/bash", "-c", reference_command], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()

        # Compute the diff
        with reference_output.open() as f:
            reference_lines = f.readlines()
        with validation_output.open() as f:
            validation_lines = f.readlines()

        result = list(unified_diff(reference_lines, validation_lines, fromfile="reference", tofile="validation"))
        if not result:
            self.logger.info("Passed!\n")
        else:
            # print CI helper directly to avoid logger decorations
            print(f"ATLAS-CI-ADD-LABEL: {test.run.value}-{test.type.value}-output-changed")
            print()

            self.logger.error(f"Your change breaks the frozen tier0 policy in test {test.ID}:")
            for line in result:
                self.logger.info(f"  {line.strip()}")
            self.logger.info("-----------------------------------------------------\n")

        return not result


class SimpleCheck(WorkflowCheck):
    """Run A Very Simple Check."""

    def __init__(self, setup: TestSetup, name: str, quantity: str, unit: str, field: int, threshold: float):
        super().__init__(setup)
        self.name = name
        self.quantity = quantity
        self.unit = unit
        self.field = field
        self.threshold = threshold

    def run(self, test: WorkflowTest) -> bool:
        self.logger.info("-----------------------------------------------------")
        self.logger.info(f"Running {test.ID} {self.name} Check")

        result = True
        for step in test.steps:
            log_name = f"log.{step}"
            reference_log = test.reference_path / log_name
            validation_log = test.validation_path / log_name

            reference_value = 0
            with reference_log.open() as file:
                found = False
                for line in file:
                    if self.quantity in line:
                        reference_value = float(line.split()[self.field])
                        found = True
                        break
                if not found:
                    self.logger.error(f"No data available in {reference_log}. Job failed.")
                    return False

            validation_value = 0
            with validation_log.open() as file:
                found = False
                for line in file:
                    if self.quantity in line:
                        validation_value = float(line.split()[self.field])
                        found = True
                        break
                if not found:
                    self.logger.error(f"No data available in {validation_log}. Job failed.")
                    return False

            if reference_value != 0:
                factor = validation_value / reference_value

                # Error if the factor increases (very bad things)
                # Warning if the factor decreases (should be an understood feature)
                if factor > 1. + self.threshold:
                    self.logger.error(f"{self.quantity} in the {step} step with(out) your change is {validation_value} ({reference_value}) {self.unit}")
                    self.logger.error(f"Your change changes {self.quantity} by a factor {factor}")
                    self.logger.error("Is this an expected outcome of your change(s)?")
                    result = False
                    self.logger.error(f"{step}: {self.name}")
                    self.logger.error(f"ref  {reference_value} {self.unit}")
                    self.logger.error(f"val {validation_value} {self.unit}")
                if factor < 1. - self.threshold:
                    self.logger.warning(f"{self.quantity} in the {step} step with(out) your change is {validation_value} ({reference_value}) {self.unit}")
                    self.logger.warning(f"Your change changes {self.quantity} by a factor {factor}")
                    self.logger.warning("Is this an expected outcome of your change(s)?")
                    result = True
                    self.logger.warning(f"{step}: {self.name}")
                    self.logger.warning(f"ref  {reference_value} {self.unit}")
                    self.logger.warning(f"val {validation_value} {self.unit}")

        if result:
            self.logger.info("Passed!\n")
        else :
            self.logger.error("Failed!\n")

        return result


class WarningsComparisonCheck(WorkflowCheck):
    """Run WARNINGS check."""

    def run(self, test: WorkflowTest):
        self.logger.info("-----------------------------------------------------")
        self.logger.info(f"Running {test.ID} WARNINGS Check\n")

        result = True
        for step in test.steps:
            log_name = f"log.{step}"
            reference_log = test.reference_path / log_name
            validation_log = test.validation_path / log_name
            warnings_reference = warnings_count(reference_log)
            warnings_validation  = warnings_count (validation_log)

            wr=[]
            for w in warnings_reference:
                wr.append(w[9:])
            wv=[]
            for w in warnings_validation:
                wv.append(w[9:])

            wn = list(set(wv)-set(wr))
            wo = list(set(wr)-set(wv))


            if len(warnings_validation) > len(warnings_reference):
                self.logger.error(f"Validation log file {validation_log} has {len(warnings_validation) - len(warnings_reference)} more warning(s) than the reference log file {reference_log}")
                self.logger.error("Please remove the new warning message(s):")
                for w in wn:
                    self.logger.error(w)
                result = False
            elif len(warnings_validation) < len(warnings_reference):
                self.logger.info(f"Validation log file {validation_log} has {len(warnings_reference) - len(warnings_validation)} less warnings than the reference log file {reference_log}")
                self.logger.info("The reduction of unnecessary WARNINGs is much appreciated. Is it expected?")
                self.logger.info("The following warning messages have been removed:")
                for w in wo:
                    self.logger.info(w)
                result = True
            else :
                self.logger.info(f"Validation log file {validation_log} has the same number of warnings as the reference log file {reference_log}")
                result = True

        if result:
            self.logger.info("Passed!\n")
        else :
            self.logger.error("Failed!\n")

        return result


class FPECheck(WorkflowCheck):
    """Run FPE check."""

    def run(self, test: WorkflowTest):
        self.logger.info("-----------------------------------------------------")
        self.logger.info(f"Running {test.ID} FPE Check")

        result = True
        for step in test.steps:
            log = test.validation_path / f"log.{step}"
            fpes = {}
            with log.open() as file:
                for line in file:
                    if "WARNING FPE" in line:
                        fpe = line.split()[10].replace("[", "").replace("]", "")
                        if fpe in fpes:
                            fpes[fpe] += 1
                        else:
                            fpes[fpe] = 1
            if fpes.keys():
                result = False
                self.logger.error(f" {step} validation test step FPEs")
                for fpe, count in sorted(fpes.items(), key=lambda item: item[1]):
                    self.logger.error(f"{count:>5}  {fpe}")

        if result:
            self.logger.info("Passed!\n")
        else :
            self.logger.error("Failed!\n")

        return result
