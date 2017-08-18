/*!
 * @file unit_defs.h
 * @author Segev BenZvi
 * @date 19 Jun 2011
 * @brief X-macro unit definitions, for use in unit definition lists and
 *        dictionaries
 * @version $Id: unit_defs.h 37108 2017-01-24 14:36:05Z imc $
 *
 * This file contains names and numerical definitions for units used in AERIE
 * software.  The definitions are written as X-macros so that they can be
 * expanded into const double definitions or written as a string-double
 * dictionary.  The latter is useful for exposing units to configuration file
 * parsers (such as XML) or creating python bindings.
 * 
 * The use of X-macros ensures that the unit names and values stay in sync
 * between the definition lists and dictionaries in the software.  This comes
 * at the cost of clarity.  For the confused, consider the case of a
 * definition list.  The X-macros below can be transformed into something like
 * this with an appropriate macro definition:
 *
 * @code
 * X(meter, 1.0)       --> const double meter = 1.0;
 * X(cm, 1e-2 * meter) --> const double cm = 1e-2 * meter;
 * X(km, 1e3 * meter)  --> const double km = 1e3 * meter;
 * ...
 * @endcode
 *
 * Note that the base units are defined in a self-consistent way as:
 * <table>
 *   <tr><td> meter              </td><td> (meter)      </td></tr>
 *   <tr><td> nanosecond         </td><td> (nanosecond) </td></tr>
 *   <tr><td> electron Volt      </td><td> (eV)         </td></tr>
 *   <tr><td> positron charge    </td><td> (eplus)      </td></tr>
 *   <tr><td> degree Kelvin      </td><td> (kelvin)     </td></tr>
 *   <tr><td> amount of stuff    </td><td> (mole)       </td></tr>
 *   <tr><td> luminous intensity </td><td> (lux)        </td></tr>
 *   <tr><td> radian             </td><td> (radian)     </td></tr>
 *   <tr><td> steradian          </td><td> (steradian)  </td></tr>
 * </table>
 */

// _______________________
// Mathematical constants \_____________________________________________________
X(euler,        2.7182818284590452353602)
X(ln10,         2.3025850929940456840180)
X(pi,           3.1415926535897932384626)
X(twopi,        2. * pi)
X(halfpi,       0.5 * pi)
X(infinity,     std::numeric_limits<double>::infinity())

// ________________
// Metric prefixes \____________________________________________________________
X(yocto,        1e-24)
X(zepto,        1e-21)
X(atto,         1e-18)
X(femto,        1e-15)
X(pico,         1e-12)
X(nano,         1e-9)
X(micro,        1e-6)
X(milli,        1e-3)
X(centi,        1e-2)
X(deci,         1e-1)
X(deka,         1e+1)
X(hecto,        1e+2)
X(kilo,         1e+3)
X(Mega,         1e+6)
X(Giga,         1e+9)
X(Tera,         1e+12)
X(Peta,         1e+15)
X(Exa,          1e+18)
X(Zetta,        1e+21)
X(Yotta,        1e+24)

// ______________
// Angular units \______________________________________________________________
X(radian,       1.)
X(rad,          radian)

X(milliradian,  milli * radian)
X(mrad,         milliradian)

X(degree,       pi / 180.0 * radian)
X(deg,          degree)
X(arcminute,    degree / 60.)
X(arcmin,       arcminute)
X(arcsecond,    arcminute / 60.)
X(arcsec,       arcsecond)

X(steradian,    1.)
X(sr,           steradian)

// _____________
// Length units \_______________________________________________________________
X(meter,        1.)
X(m,            meter)
X(meter2,       meter * meter)
X(meter3,       meter2 * meter)

X(fermi,        femto * meter)
X(angstrom,     1e-10 * meter)

X(nanometer,    nano * meter)
X(nanometer2,   nanometer * nanometer)
X(nanometer3,   nanometer2 * nanometer)
X(nm,           nanometer)
X(nm2,          nanometer2)
X(nm3,          nanometer3)

X(micrometer,   micro * meter)
X(micrometer2,  micrometer * micrometer)
X(micrometer3,  micrometer2 * micrometer)
X(um,           micrometer)
X(um2,          micrometer2)
X(um3,          micrometer3)

X(millimeter,   milli*meter)
X(millimeter2,  millimeter * millimeter)
X(millimeter3,  millimeter2 * millimeter)
X(mm,           millimeter)
X(mm2,          millimeter2)
X(mm3,          millimeter3)

X(centimeter,   centi * meter)
X(centimeter2,  centimeter * centimeter)
X(centimeter3,  centimeter2 * centimeter)
X(cm,           centimeter)
X(cm2,          centimeter2)
X(cm3,          centimeter3)

X(kilometer,    kilo * meter)
X(kilometer2,   kilometer * kilometer)
X(kilometer3,   kilometer2 * kilometer)
X(km,           kilometer)
X(km2,          kilometer2)
X(km3,          kilometer3)

X(barn,         1e-28 * meter2)
X(millibarn,    milli * barn)
X(microbarn,    micro * barn)
X(nanobarn,     nano * barn)
X(picobarn,     pico * barn)

X(inch,         2.54 * cm)
X(foot,         12 * inch)
X(yard,         3 * foot)
X(mile,         1760 * yard)

X(au,           149597870660. * meter)
X(parsec,       au / arcsecond)
X(pc,           parsec)
X(kpc,          kilo*parsec)
X(Mpc,          Mega*parsec)
X(Gpc,          Giga*parsec)

// _________________________
// Time and frequency units \___________________________________________________
X(nanosecond,   1.)
X(ns,           nanosecond)

X(second,       1e9 * nanosecond)
X(s,            second)

X(picosecond,   pico * second)
X(ps,           picosecond)

X(microsecond,  micro * second)

X(millisecond,  milli * second)
X(ms,           millisecond)

X(minute,       60. * second)
X(hour,         60. * minute)
X(day,          24. * hour)

X(siderealDay,  86164.09054 * second)

X(hertz,        1. / second)
X(Hz,           hertz)

X(kilohertz,    kilo * hertz)
X(kHz,          kilohertz)

X(megahertz,    Mega * hertz)
X(MHz,          megahertz)

X(gigahertz,    Giga * hertz)
X(GHz,          gigahertz)

// ____________________________
// Electric charge and current \________________________________________________
X(eplus,        1.)
X(eSI,          1.602176462e-19)
X(coulomb,      eplus / eSI)

X(ampere,       coulomb / second)

X(nanoampere,   nano * ampere)
X(nA,           nanoampere)

X(microampere,  micro * ampere)

X(milliampere,  milli * ampere)
X(mA,           milliampere)

// _________________
// Energy and power \___________________________________________________________
X(electronvolt, 1.)
X(eV,           electronvolt)
X(meV,          milli * electronvolt)
X(keV,          kilo * electronvolt)
X(MeV,          Mega * electronvolt)
X(GeV,          Giga * electronvolt)
X(TeV,          Tera * electronvolt)
X(PeV,          Peta * electronvolt)
X(EeV,          Exa * electronvolt)

X(joule,        electronvolt / eSI)
X(erg,          1e-7 * joule)
X(Calorie,      4184 * joule)
X(calorie,      1e-3 * Calorie)

X(watt,         joule / second)
X(W,            watt)

// _____
// Mass \_______________________________________________________________________
X(kilogram,     joule * second*second / (meter2))
X(kg,           kilogram)

X(gram,         1e-3 * kilogram)
X(g,            gram)

X(milligram,    milli * gram)
X(mg,           milligram)

// ______
// Force \______________________________________________________________________
X(newton,       joule / meter)
X(dyne,         erg / cm)

// _________
// Pressure \___________________________________________________________________
X(pascal,       newton / meter2)
X(bar,          1e5 * pascal)

X(millibar,     milli * bar)
X(mbar,         millibar)

X(hectopascal,  hecto * pascal)
X(hPa,          hectopascal)

// ________________________________________________
// Electric potential, resistance, and capacitance \____________________________
X(megavolt,     MeV / eplus)
X(MV,           megavolt)
X(kilovolt,     1e-3 * megavolt)
X(kV,           kilovolt)
X(volt,         1e-3 * kilovolt)
X(V,            volt)
X(millivolt,    milli * volt) 
X(mV,           millivolt)
X(microvolt,    micro * volt) 
X(uV,           microvolt)
X(nanovolt,     nano * volt) 
X(nV,           nanovolt)

X(ohm,          volt / ampere)

X(farad,        coulomb / volt)
X(millifarad,   milli * farad)
X(mF,           millifarad)
X(microfarad,   micro * farad)
X(uF,           microfarad)
X(nanofarad,    nano * farad)
X(nF,           nanofarad)
X(picofarad,    pico * farad)
X(pF,           picofarad)

// ________________________
// Magnetic field and flux \____________________________________________________
X(weber,        volt * second)
X(tesla,        volt * second / meter2)
X(gauss,        1e-4 * tesla)

// ____________
// Temperature \________________________________________________________________
X(kelvin,       1.)

// ________________
// Amount of stuff \____________________________________________________________
X(mole,         1.)
X(mol,          mole)

// __________________________________________
// Luminous intensity, flux, and illuminance \__________________________________
X(candela,      1.)
X(lumen,        candela * steradian)
X(lux,          lumen / meter2)

// _____________________
// Radioactivity/dosage \_______________________________________________________
X(becquerel,    1./second)
X(curie,        3.7e10 * becquerel)
X(gray,         joule / kilogram)

// ____________________
// Other handy numbers \________________________________________________________
X(perCent,      1e-2)
X(perMille,     1e-3)
X(perMillion,   1e-6)
X(perBillion,   1e-9)

