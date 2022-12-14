All sequences in [MuonMenuSequences](MuonMenuSequences.py) are organized into an 'Alg' sequence which sets up the event view creator algorithm and all reconstruction algorithms needed for the seuqence, and a menu sequence that retrieves the Alg sequence and adds the HypoAlg to complete the menu sequence.

Fast SA (MuFast) Sequences
-----

* muFastSequence
  * Runs the default muFast algorithm in L1 RoIs without overlap removal
  * Default for single muon triggers and Bphysics triggers
* muFastOvlpRmSequence
  * Runs the default muFast algorithm in L1 RoIs with overlap removal
  * Default for multi-muon triggers (outside of Bphysics)
* mul2mtSAOvlpRmSequence
  * Runs muFast in multi-track mode with overlap removal
  * multi-track mode improves efficiency for nearby muons
  * Chains have 'l2mt' in the name

Fast CB (MuComb) Sequences
-----

* muCombSequence
  * Runs fast ID tracking and muComb algorithm without overlap removal
  * Uses RoIs generated by muFast algorithm centered on muFast muons
  * Bphysics triggers run only the ID tracking and skip muComb reconstruction
  * Default for single muon triggers and Bphysics triggers
* muCombOvlpRmSequence
  * Runs fast ID tracking and muComb algorithm with overlap removal
  * Uses RoIs generated by muFast algorithm centered on muFast muons
  * Default for multi-muon triggers (outside of Bphysics)
* muCombLRTSequence
  * Runs fast ID large radius tracking and muComb algorithm without overlap removal
  * Runs in RoIs centered on muFast muons with a eta/phi widths optimised for LRT
  * Used in chains that have 'LRT' in the name
* mul2IOOvlpRmSequence
  * Runs fast ID tracking and muFast alg in inside-out mode (extrapolates ID tracks to muon spectrometer) with overlap removal
  * Runs in RoIs generated by first pass of muFast algorithm, and skips muComb reconstruction
  * Recovers cases where there are multiple muons in a single RoI that the standard muFast algorithm reconstructs as the same muon
  * Runs in chains with 'l2io' in name and most Bphysics triggers by default
* mul2mtCBOvlpRmSequence
  * Runs fast ID tracking and muComb algorithm with overlap removal
  * Same as muCombOvlpRmSequence except uses muon reconstructed from muFast algo in multi-track mode
  * Runs in chains with 'l2mt' in chain name

Precision SA Sequences
-----

* muEFSASequence
  * Runs precision MS track reconstruction using offline algorithms
  * Runs in RoIs created by muFast algorithm centered on muFast muons created before extrapolation to the IP
  * Runs in all standard single and multi-muon (including ms-only) RoI based triggers
* muEFSAFSSequence
  * Runs precision MS track reconstruction using offline algorithms in full detector (Full Scan RoI)
  * Runs in chains with 'noL1' in chain name

Precision CB Sequences
-----

* muEFCBSequence
  * Runs the precision ID tracking and precision MS-ID combination using offline algorithms
  * Uses same RoIs as muComb sequence
  * If no muons are reconstructed, a recovery mechanism that runs the inside-out reconstruction (ID tracks are extrapolated to segments in MS) runs
  * Runs in all standard single and multi-muon RoI based triggers except ms-only triggers

* muEFCBLRTSequence
  * Runs the precision large radius tracking and repcision MS-ID combination (same muon reconstruction as muEFCBSequence)
  * Uses same RoIs as muComb LRT sequence
  * Runs in triggers with 'LRT' in name

* muEFCBFSSequence
  * Runs the ID tracking (fast+precision) and MS-ID precision reconstruction using offline algorithms
  * Runs in RoIs centered on muons created in muEFSAFSSequence
  * Unlike the RoI based MS-ID reconstruction, the inside-out recovery does not run

Other Sequences
----

* efLateMuRoISequence
  * Fetches the muon RoIs and checks that there is at least one RoI from the next bunch crossing
  * Used only in Late muon trigger
* efLateMuSequence
  * Runs the ID tracking in the late muon RoIs and reconstructs the late muons using the offline inside-out algorithms configured for slow particles.
  * Used only in Late muon trigger
* muEFIsoSequence
  * Runs the track isolation in a cone around the combined muons
  * ID tracking is run in larger RoI than the standard muon RoIs
  * Runs in triggers that specify isolation (typically ivarX)
* muEFMSIsoSequence
  * Runs the track isolation in a cone around MS-only muons
  * ID tracking is run in larger RoI than the standard muon RoIs
  * Runs in triggers that specify isolation (typically iXms)  