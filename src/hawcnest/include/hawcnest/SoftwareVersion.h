/*!
 * @file SoftwareVersion.h
 * @brief Macros for coding and decoding the software version.
 * @author Segev BenZvi
 * @date 2 Aug 2010
 * @version $Id: SoftwareVersion.h 14858 2013-04-27 14:12:23Z sybenzvi $
 */

#ifndef HAWCNEST_SOFTWAREVERSION_H_INCLUDED
#define HAWCNEST_SOFTWAREVERSION_H_INCLUDED

#include <hawcnest/config.h>
#include <iostream>
#include <iomanip>

/*!
 * @def SOFTWARE_VERSION_CODE
 * @author Segev BenZvi
 * @ingroup hawcnest_api
 * @date 2 Aug 2010
 * @brief Macro to pack software major, minor, and patch versions into a single
 *        number; for comparisons run by the preprocessor
 */
#ifndef SOFTWARE_VERSION_CODE
#define SOFTWARE_VERSION_CODE(major, minor, patch) \
  (((major) << 16) + ((minor) << 8) + (patch))
#endif

/*!
 * @def SOFTWARE_DECODE_MAJOR_VERSION
 * @author Segev BenZvi
 * @ingroup hawcnest_api
 * @date 2 Aug 2010
 * @brief Unpack a major version number from a software version code
 */
#ifndef SOFTWARE_DECODE_MAJOR_VERSION
#define SOFTWARE_DECODE_MAJOR_VERSION(code) \
  (((code) >> 16) & 0xff)
#endif

/*!
 * @def SOFTWARE_DECODE_MINOR_VERSION
 * @author Segev BenZvi
 * @ingroup hawcnest_api
 * @date 2 Aug 2010
 * @brief Unpack a minor version number from a software version code
 */
#ifndef SOFTWARE_DECODE_MINOR_VERSION
#define SOFTWARE_DECODE_MINOR_VERSION(code) \
  (((code) >> 8) & 0xff)
#endif

/*!
 * @def SOFTWARE_DECODE_PATCH_VERSION
 * @author Segev BenZvi
 * @ingroup hawcnest_api
 * @date 2 Aug 2010
 * @brief Unpack a patch number from a software version code
 */
#ifndef SOFTWARE_DECODE_PATCH_VERSION
#define SOFTWARE_DECODE_PATCH_VERSION(code) \
  ((code) & 0xff)
#endif

/*!
 * @def AERIE_VERSION_CODE
 * @author Segev BenZvi
 * @ingroup hawcnest_api
 * @date 2 Aug 2010
 * @brief Packed version code for the current release of aerie.  Can be used 
 *        in preprocessor comparisons or at run-time with the SoftwareVersion
 *        class
 *
 * Example compile-time usage:
 * \code
 * #if AERIE_VERSION_CODE > SOFTWARE_VERSION_CODE(0, 9, 2)
 *   // Compile code in this block if the package is recent enough...
 * #endif
 * \endcode
 */
#ifndef AERIE_VERSION_CODE
#define AERIE_VERSION_CODE \
  (SOFTWARE_VERSION_CODE(AERIE_MAJOR_VERSION, AERIE_MINOR_VERSION, AERIE_PATCH_VERSION))
#endif

/*!
 * @class SoftwareVersion
 * @author Segev BenZvi
 * @ingroup hawcnest_api
 * @date 2 Aug 2010
 * @brief Run-time bit packing of software major, minor, and patch version into
 *        a single number that can be compared with boolean operators
 * @todo Include package name strings and use them in the comparison? 
 *
 * Example run-time usage:
 * \code
 * cin >> M >> N >> P;
 * if (AERIE_VERSION_CODE > SoftwareVersion(M, N, P) {
 *   // Do something at run time if the version is new enough...
 * }
 * \endcode
 */
class SoftwareVersion {

  public:

    typedef unsigned int version_t;

    /// Construction from major, minor, and patch numbers
    SoftwareVersion(version_t major, version_t minor, version_t patch) :
      version_(SOFTWARE_VERSION_CODE(major, minor, patch)) { }

    /// Explicit or implicit construction from an existing version code (e.g.,
    /// something packed at compile time by the SOFTWARE_VERSION_CODE macro)
    SoftwareVersion(version_t code) :
      version_(code) { }

   ~SoftwareVersion() { }

    version_t GetMajorVersion() const
    { return SOFTWARE_DECODE_MAJOR_VERSION(version_); }

    version_t GetMinorVersion() const
    { return SOFTWARE_DECODE_MINOR_VERSION(version_); }

    version_t GetPatchVersion() const
    { return SOFTWARE_DECODE_PATCH_VERSION(version_); }

  private:

    version_t version_;

  // Boolean comparisons
  friend bool operator<(const SoftwareVersion& a, const SoftwareVersion& b)
  { return a.version_ < b.version_; }

  friend bool operator>(const SoftwareVersion& a, const SoftwareVersion& b)
  { return a.version_ > b.version_; }

  friend bool operator<=(const SoftwareVersion& a, const SoftwareVersion& b)
  { return !(a.version_ > b.version_); }

  friend bool operator>=(const SoftwareVersion& a, const SoftwareVersion& b)
  { return !(a.version_ < b.version_); }

  friend bool operator==(const SoftwareVersion& a, const SoftwareVersion& b)
  { return a.version_ == b.version_; }

  friend bool operator!=(const SoftwareVersion& a, const SoftwareVersion& b)
  { return !(a.version_ == b.version_); }

  friend std::ostream& operator<<(std::ostream& os, const SoftwareVersion& s)
  {
    os << s.GetMajorVersion() << '.' << std::setfill('0')
       << std::setw(2) << s.GetMinorVersion() << '.'
       << std::setw(2) << s.GetPatchVersion() << std::setfill(' ');
    return os;
  }

};

#endif // HAWCNEST_SOFTWAREVERSION_H_INCLUDED

