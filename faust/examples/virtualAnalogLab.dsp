declare guid "ExVL";
declare name "virtualAnalogLab";
declare URL "https://faustdoc.grame.fr/examples/generator/";

import("stdfaust.lib");

process = 
 vgroup("[1]", dm.virtual_analog_oscillator_demo) : 
 vgroup("[2]", dm.moog_vcf_demo) : 
 vgroup("[3]", dm.spectral_level_demo)
 // See also: vgroup("[3]", dm.fft_spectral_level_demo(32))
  <: _,_;
  