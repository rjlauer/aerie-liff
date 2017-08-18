
/*!
 * @file constant_defs.h
 * @author Segev BenZvi
 * @date 11 Apr 2012
 * @brief X-macro definitions of physical constants derived from the base units
 *        in the hawcnest project
 *
 * This file contains names and numerical definitions for physics constants
 * used in the AERIE software.  The definitions are written as X-macros so they
 * can be expanded into const double definitions or exposed to python using C
 * preprocessor magic.  The use of X-macros just ensures that the unit names
 * and bindings stay in sync as more units are added, at some mild cost to
 * clarity.
 *
 * For the confused, consider the case of a unit definition list.  With an
 * appropriate macro definition the X-macros below can be transformed into
 * something like this:
 *
 * @code
 * X(hbar, h/twopi, Planck constant) --> const double hbar = h/twopi;
 * @endcode
 *
 * Note that values used here have been taken from the tables maintained by the
 * <a href="http://pdg.lbl.gov">Particle Data Group</a> (2011).
 *
 * @version $Id: constant_defs.h 14879 2013-04-27 16:21:17Z sybenzvi $
 */

// __________________________
// Electromagnetic constants \__________________________________________________
X(c,         299792458. * meter/second,       Speed of light)
X(mu0,       4.*pi*1e-8 * dyne/(mA*mA),       Vacuum permeability)
X(epsilon0,  1./(mu0*c*c),                    Vacuum permittivity)
X(ke,        1./(4.*pi*epsilon0),             Electric force constant)

// _______________________________
// Relativistic/quantum constants \_____________________________________________
X(h,         6.62606896e-34 * joule*second,   Planck constant)
X(hbar,      h / twopi,                       Reduced Planck constant)
X(alpha,     ke*eplus*eplus/(hbar*c),         Fine structure constant)

// ________________________
// Gravitational constants \____________________________________________________
X(G,         6.67428e-11 * meter3/(kg*s*s),   Newtonian gravitational constant)
X(g,         9.80665 * meter/(s*s),           Std gravitational acceleration)

// ___________________________________
// Thermodynamic and atomic constants \_________________________________________
X(Na,        6.02214179e23 / mole,            Avogadro number)
X(kB,        1.3806504e-23 * joule/kelvin,    Boltzmann constant)
X(Rgas,      Na*kB,                           Molar gas constant)
X(dalton,    1e-3/Na*kilogram,                Dalton (amu): Metrologia 49:487)
X(Da,        dalton,                          Dalton abbreviation)
X(u,         dalton,                          Unified atomic mass unit = Dalton)

// __________________________
// Basic particle properties \__________________________________________________
X(Mp,        938.272013 * MeV/(c*c),          Proton mass)
X(Mn,        939.565379 * MeV/(c*c),          Neutron mass)
X(Md,        1875.612793 * MeV/(c*c),         Deuteron mass)
X(Me,        0.510998910 * MeV/(c*c),         Electron mass)
X(Mmu,       105.6583715 * MeV/(c*c),         Muon mass)
X(Mtau,      1776.82 * MeV/(c*c),             Tau mass)

X(Re,        ke*eplus*eplus/(Me*c*c),         Classical electron radius)
X(Rbohr,     Re / (alpha*alpha),              Bohr radius)
X(lcompton,  h / (Me*c),                      Compton wavelength)
X(rydberg,   0.5*Me*c*c*alpha*alpha,          Rydberg constant)

X(muB,       eplus*hbar / (2*Me),             Bohr magneton)
X(muN,       eplus*hbar / (2*Mp),             Nuclear magneton)

