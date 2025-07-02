# Flux - Dynamic Matrix Mixer
Author: Nathan Harris
Requires API version 9 - firmware 1.10 beta1

Flux is a versatile N x M matrix mixer designed for creating dynamic, evolving signal routings and textures. Go beyond static patches with triggered sequencing, smooth interpolation, and adjustable transition times and curves.

Features:

- Flexible N x M Matrix Mixing (up to 6x6, set via Specifications).
- Dynamic X/Y Shifting via Manual Offset parameters (-100% to +100%).
- Triggered Sequencing: Step input advances internal X/Y shift state.
- Sequence Reset: Dedicated trigger input resets internal shift state to zero.
- Adjustable Step Size: X/Y Increment parameters control triggered shift amount (-100% to +100%).
- Smooth Transitions: Bilinear interpolation calculates effective gains during shifts.
- Global Asymmetric Slew: Control the attack ('Slew Up') and decay ('Slew Down') time (ms) for all gain changes.
- Gain Shape Control: Adjust the curve of gain transitions from logarithmic (-ve values) through linear (0) to exponential (+ve values).


