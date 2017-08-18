/*!
 * @file FilePolicies.h
 * @brief Policies for streaming data from files in various formats.
 * @author Segev BenZvi
 * @version $Id: FilePolicies.h 16844 2013-08-17 06:27:51Z sybenzvi $
 */

#ifndef DATACLASSES_IO_FILEPOLICIES_H_INCLUDED
#define DATACLASSES_IO_FILEPOLICIES_H_INCLUDED

#include <fstream>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/operations.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

typedef boost::iostreams::filtering_stream<boost::iostreams::input>
  FilterStream;

typedef boost::shared_ptr<FilterStream>
  FilterStreamPtr;

typedef boost::shared_ptr<std::ifstream>
  IfstreamPtr;

/*!
 * @class FilterStreamContainer
 * @author Segev BenZvi
 * @ingroup design_patterns
 * @brief Stores pointers to filtering_stream and a file stream.  Useful for
 *        decompressing a file using the filtering_stream
 *
 * This class encapsulates a file stream and a boost::filtering_stream (for
 * input files only).  It allows the creation of a persistent filtering_stream
 * that can be used by other objects, such as file I/O policy classes.  It's
 * main purpose is to create pointers to an ifstream and a filtering_stream
 * that don't go out of scope prematurely.
 *
 * The de-referencing and pointer operators are overloaded to provide access to
 * all the members of the filtering stream.
 */
class FilterStreamContainer {
  public:
    FilterStreamContainer(IfstreamPtr i, FilterStreamPtr f) : i_(i), f_(f) { }
    FilterStreamPtr operator->() { return f_; }
    FilterStream& operator*() { return *f_; }
  private:
    IfstreamPtr i_;
    FilterStreamPtr f_;
};

/*!
 * @class GZipFilePolicy
 * @author Segev BenZvi
 * @ingroup design_patterns
 * @brief Encapsulate streaming of a gzipped file
 *
 * A filtering_stream is used to access a input file stream with a
 * decompressing filter.  Pointers to the filtering_stream and the file stream
 * must be stored (i.e., kept in scope) while the file is being read.  This
 * policy handles files compressed using gzip.
 */
class GZipFilePolicy {
  public:
    static FilterStreamContainer open(const std::string& filename) { 
      FilterStreamPtr in = boost::make_shared<FilterStream>();
      IfstreamPtr ifstr = boost::make_shared<std::ifstream>(filename.c_str(), std::ios::binary);
      in->push(boost::iostreams::gzip_decompressor());
      in->push(*ifstr);
      return FilterStreamContainer(ifstr, in);
    }
};

/*!
 * @class BZip2FilePolicy
 * @author Segev BenZvi
 * @ingroup design_patterns
 * @brief Encapsulate streaming of a bzipped file
 *
 * A filtering_stream is used to access a input file stream with a
 * decompressing filter.  Pointers to the filtering_stream and the file stream
 * must be stored (i.e., kept in scope) while the file is being read.  This
 * policy handles files compressed using bzip2.
 */
class BZip2FilePolicy {
  public:
    static FilterStreamContainer open(const std::string& filename) { 
      FilterStreamPtr in = boost::make_shared<FilterStream>();
      IfstreamPtr ifstr = boost::make_shared<std::ifstream>(filename.c_str(), std::ios::binary);
      in->push(boost::iostreams::bzip2_decompressor());
      in->push(*ifstr);
      return FilterStreamContainer(ifstr, in);
    }
};

/*!
 * @class TextFilePolicy
 * @author Segev BenZvi
 * @ingroup design_patterns
 * @brief Encapsulate streaming of a plaintext file
 */
class TextFilePolicy {
  public:
    static IfstreamPtr open(const std::string& filename) {
      IfstreamPtr ifstr = boost::make_shared<std::ifstream>(filename.c_str());
      return ifstr;
    }
};

#endif // DATACLASSES_IO_FILEPOLICIES_H_INCLUDED

