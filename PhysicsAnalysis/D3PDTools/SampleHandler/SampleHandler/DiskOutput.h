/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @author Nils Krumnack


#ifndef SAMPLE_HANDLER__DISK_OUTPUT_H
#define SAMPLE_HANDLER__DISK_OUTPUT_H


#include <SampleHandler/Global.h>

#include <TObject.h>
#include <memory>

namespace SH
{
  /// \brief a class/interface representing an output location for
  /// files
  ///
  /// This is primarily used to allow placing EventLoop outputs in
  /// other locations than the submit-directory.  This is particularly
  /// useful for placing large n-tuples directly onto a storage
  /// element, instead of putting them onto a local disk that has
  /// neither the capacity nor the bandwith.
  ///
  /// In principle this ought to be handle to any kind of storage
  /// element, but for now (04 Feb 19) it only supports local disks
  /// and xrootd servers.  If you need anything else ask for it.
  ///
  /// Files created are identified both by a sample name and a segment
  /// identifier.  The sample name is what's configured in the
  /// EventLoop job.  The segment identifier is generated by EventLoop
  /// if more than one job is run per sample.
  ///
  /// This class derives from \ref TObject to allow streaming the
  /// objects as part of the EventLoop job configuration.  As such all
  /// derived objects need to be designed to be streamable.
  ///
  /// \warn This interface is not considered fixed, it can change as
  /// the needs of EventLoop change or as more output location types
  /// are added.

  class DiskOutput : public TObject
  {
    //
    // public interface
    //

    /// \brief test the invariant of this object
    /// \par Guarantee
    ///   no-fail
  public:
    void testInvariant () const;


    /// \brief standard constructor
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory I
  public:
    DiskOutput ();


    /// \brief create a \ref DiskWriter object managing an output file
    ///
    /// \pre !sampleName.empty()
    /// \post result != nullptr
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II\n
    ///   i/o errors
  public:
    std::unique_ptr<DiskWriter>
    makeWriter (const std::string& sampleName,
                const std::string& segmentName,
                const std::string& suffix) const;


    /// \brief the final output location for the given segment
    ///
    /// \pre !sampleName.empty()
    /// \post !result.empty()
    /// \par Guarantee
    ///   strong
    /// \par Failures
    ///   out of memory II
  public:
    std::string
    targetURL (const std::string& sampleName,
               const std::string& segmentName,
               const std::string& suffix) const;



    //
    // virtual interface
    //

    /// \copydoc makeWriter()
    /// \par Rationale
    ///   the virtual part of DiskOutput::makeWriter
  protected:
    virtual std::unique_ptr<DiskWriter>
    doMakeWriter (const std::string& sampleName,
                  const std::string& segmentName,
                  const std::string& suffix) const = 0;

    /// \copydoc finalLocation()
    /// \par Rationale
    ///   the virtual part of DiskOutput::targetURL
  protected:
    virtual std::string
    getTargetURL (const std::string& sampleName,
                  const std::string& segmentName,
                  const std::string& suffix) const = 0;



    //
    // private interface
    //

    ClassDef (DiskOutput, 1);
  };
}

#endif
