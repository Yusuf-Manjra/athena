# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
import logging
import threading
from argparse import ArgumentParser, Namespace
from os import environ
from pathlib import Path
from sys import exit
from typing import List

from .Checks import FailedOrPassedCheck, SimpleCheck, WarningsCheck
from .Inputs import references_CVMFS_path
from .Test import TestSetup, WorkflowCheck, WorkflowTest


def setup_logger(name: str) -> logging.Logger:
    # Setup global logging
    logging.basicConfig(level=logging.INFO,
                        format="%(asctime)s %(levelname)-8s %(message)s",
                        datefmt="%m-%d %H:%M",
                        filename=f"./{name}.log",
                        filemode="w")
    console = logging.StreamHandler()
    console.setLevel(logging.INFO)
    formatter = logging.Formatter("%(levelname)-8s %(message)s")
    console.setFormatter(formatter)
    logger = logging.getLogger("")
    logger.addHandler(console)
    return logger


def setup_parser() -> ArgumentParser:
    parser = ArgumentParser()
    common = parser.add_argument_group("common")
    common.add_argument("-e", "--extra", type=str, dest="extra_args", default="",
                        help="Define additional args to pass e.g. --preExec 'r2e':'...' ")
    common.add_argument("-f", "--fast", action="store_true", dest="fast_mode", default=False,
                        help="""Fast option will run all q tests simultaneously,
                                such that it will run faster if you have 4 cpu core slots on which to run. Be
                                warned! Only recommended when running on a high performance machine, not
                                lxplus!""")
    common.add_argument("-v", "--validation", action="store_true", dest="validation_only", default=False,
                        help=f"""Run validation only.
                                 File output comparisons will only be performed against pre-defined
                                 reference files stored in the directory
                                 {references_CVMFS_path}
                                 and performance comparison tests will not be run.""")
    common.add_argument("-c", "--check-only", type=str, dest="unique_ID", default=None,
                        help="Re-run only the checks.")

    advanced = parser.add_argument_group("advanced")
    advanced.add_argument("--CI", action="store_true", dest="ci_mode", default=False,
                          help="Will not setup Athena - only for CI tests!")
    advanced.add_argument("--ref", type=str, dest="reference_release", default=None,
                          help="Define a particular reference release.")
    advanced.add_argument("--val", type=str, dest="validation_release", default=None,
                          help="Define a particular validation release")
    advanced.add_argument("--reference-path", type=str, dest="reference_run_path", default="",
                          help="Specify the head directory for running the reference tests. The default is /tmp/${USER}")
    advanced.add_argument("-z", "--exclusion-lists", type=str, dest="diff_rules_path", default=".",
                          help="""Specify the directory that contains the lists of variables that will be omitted
                                while comparing the outputs. The default is ./ and the format of the files is
                                ${q-test}_${format}_diff-exclusion-list.txt, e.g. q431_AOD_diff-exclusion-list.txt.""")

    tests = parser.add_argument_group("tests")
    tests.add_argument("-t", "--test", type=str, dest="test", default=None,
                       help="Specify a test to run. Supported options are: ")
    tests.add_argument("-a", "--tag", type=str, dest="ami_tag", default=None,
                       help="Override the AMI tag of the test.")
    # shortcuts
    tests.add_argument("-s", "--sim", action="store_true", dest="simulation", default=False,
                       help="Run simulation test using Sim_tf.py")
    tests.add_argument("-o", "--overlay", action="store_true", dest="overlay", default=False,
                       help="Run overlay test using Overlay_tf.py")
    tests.add_argument("-p", "--pileup", action="store_true", dest="pileup", default=False,
                       help="Run MC reconstruction chain with pile-up")
    tests.add_argument("-r", "--reco", action="store_true", dest="reco", default=False,
                       help="Run MC reconstruction (in case the default execution also runs simulation)")                   

    return parser


def get_test_setup(name: str, options: Namespace, log: logging.Logger) -> TestSetup:
    # define test setup
    setup = TestSetup(log)
    setup.reference_run_path = Path(options.reference_run_path) if options.reference_run_path else Path(f"/tmp/{environ['USER']}")
    setup.diff_rules_path = Path(options.diff_rules_path)
    setup.disable_release_setup = options.ci_mode
    setup.validation_only = options.validation_only
    if options.unique_ID:
        setup.checks_only = True
        setup.unique_ID = options.unique_ID
    setup.parallel_execution = options.fast_mode
    # not in global setup:
    # options.extra_args

    if options.ami_tag:
        log.error("Custom AMI tags not supported yet!")
        exit(1)

    # Are we running in CI
    if setup.disable_release_setup:
        log.info("You're running in CI mode.")
        log.info("This mode assumes athena is setup w/ necessary changes and only runs validation tests.")
        log.info("Then results are checked against reference files and no performance test is run.")
        log.info("If you don't know what this mode does, you shouldn't be using it.\n")
        setup.validation_only = True

    # Does the clean run head directory exist?
    if setup.validation_only:
        log.info("You are running in validation-only mode whereby only tests against your build are being run.")
        log.info("In this mode ESD and AOD outputs are compared with pre-defined reference files found in the directory")
        log.info(f"{references_CVMFS_path}\n")
        if not Path(references_CVMFS_path).exists():
            log.error(f"Exit. Validation-only mode can only be run on nodes with access to {references_CVMFS_path}")
            exit(2)
    elif setup.reference_run_path.exists():
        log.info(f"The job unique ID is '{setup.unique_ID}' (can be used to re-run the checks)\n")
    else:
        log.error("Exit. Please specify a directory that exists for the argument of the --reference-path option\n")
        log.error(f"{name}.py --reference-path <ExistingDirectory>")
        exit(1)

    # Is an ATLAS release setup?
    if 'AtlasPatchVersion' not in environ and 'AtlasArea' not in environ and 'AtlasBaseDir' not in environ and 'AtlasVersion' not in environ:
        log.error("Exit. Please setup the an ATLAS release")
        exit(3)


    if 'AtlasPatchVersion' not in environ and 'AtlasArea' not in environ and 'AtlasBaseDir' in environ and 'AtlasVersion' not in environ:
        log.warning("Please be aware that you are running a release which seems to not be a Tier0 release, where in general q-tests are not guaranteed to work.")

    # setup reference path
    setup.reference_run_path /= f"reference_test_{setup.unique_ID}"

    # Release setup & list the packages in the local InstallArea
    setup.setup_release(options.reference_release, options.validation_release)

    # Parse test string if needed
    parse_test_string(setup, options)

    return setup


def parse_test_string(setup: TestSetup, options: Namespace) -> None:
    if not options.test:
        return

    test_string = options.test.lower()

    # simulation
    if test_string in ['s', 'sim', 'simulation', 'Sim_tf', 'Sim_tf.py']:
        options.simulation = True
        return

    # overlay
    if test_string in ['o', 'overlay', 'Overlay_tf', 'Overlay_tf.py']:
        options.overlay = True
        return

    # pile-up
    if test_string in ['p', 'pileup', 'pile-up']:
        options.pileup = True
        return

    # reco
    if test_string in ['r', 'reco', 'reconstruction', 'Reco_tf', 'Reco_tf.py']:
        options.reco = True
        return


def get_standard_performance_checks(setup: TestSetup) -> List[WorkflowCheck]:
    return [
        SimpleCheck(setup, "CPU Time"       , "evtloop_time",     "msec/event",   4, 0.4),
        SimpleCheck(setup, "Physical Memory", "VmRSS",            "kBytes",       4, 0.2),
        SimpleCheck(setup, "Virtual Memory" , "VmSize",           "kBytes",       4, 0.2),
        SimpleCheck(setup, "Memory Leak"    , "leakperevt_evt11", "kBytes/event", 7, 0.05),
        WarningsCheck(setup),
    ]


def run_tests(setup: TestSetup, tests: List[WorkflowTest]) -> None:
    if not setup.checks_only:
        threads = {}
        setup.logger.info("------------------ Run Athena workflow test jobs---------------")
        if setup.parallel_execution and not setup.validation_only:
            for test in tests:
                threads[f"{test.ID}_reference"]   = threading.Thread(target=lambda test=test: test.run_reference())
                threads[f"{test.ID}_validation"] = threading.Thread(target=lambda test=test: test.run_validation())
                threads[f"{test.ID}_reference"].start()
                threads[f"{test.ID}_validation"].start()

            for thread in threads:
                threads[thread].join()
        elif setup.validation_only:
            for test in tests:
                threads[f"{test.ID}_validation"] = threading.Thread(target=lambda test=test: test.run_validation())
                threads[f"{test.ID}_validation"].start()
                if not setup.parallel_execution:
                    threads[f"{test.ID}_validation"].join()

            if setup.parallel_execution:
                for thread in threads:
                    threads[thread].join()
        else:
            for test in tests:
                threads[f"{test.ID}_reference"]   = threading.Thread(target=lambda test=test: test.run_reference())
                threads[f"{test.ID}_validation"] = threading.Thread(target=lambda test=test: test.run_validation())
                threads[f"{test.ID}_reference"].start()
                threads[f"{test.ID}_validation"].start()
                threads[f"{test.ID}_reference"].join()
                threads[f"{test.ID}_validation"].join()


def run_checks(setup: TestSetup, tests: List[WorkflowTest], performance_checks: List[WorkflowCheck]) -> bool:
    all_passed = True
    # define common checks
    main_check = FailedOrPassedCheck(setup)
    # run checks
    for test in tests:
        all_passed = all_passed and test.run_checks(main_check, performance_checks)
    return all_passed


def run_summary(setup: TestSetup, tests: List[WorkflowTest], status: bool) -> None:
    setup.logger.info("-----------------------------------------------------")
    setup.logger.info("---------------------- Summary ----------------------")
    if status:
        setup.logger.info("ALL TESTS: PASSED (0)")
    else:
        setup.logger.error("ALL TESTS: FAILED (10)")
        exit(10)
