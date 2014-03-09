/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2014 Live Networks, Inc.  All rights reserved.
// A filter that breaks up a H.264 Video Elementary Stream into NAL units.
// Implementation

#include "H264VideoStreamFramer.hh"
#include "H264VideoRTPSource.hh" // for "parseSPropParameterSets()"

H264VideoStreamFramer* H264VideoStreamFramer
::createNew(UsageEnvironment& env, FramedSource* inputSource, Boolean includeStartCodeInOutput) {
  return new H264VideoStreamFramer(env, inputSource, True, includeStartCodeInOutput);
}

H264VideoStreamFramer
::H264VideoStreamFramer(UsageEnvironment& env, FramedSource* inputSource, Boolean createParser, Boolean includeStartCodeInOutput)
  : H264or5VideoStreamFramer(264, env, inputSource, createParser, includeStartCodeInOutput) {
}

H264VideoStreamFramer::~H264VideoStreamFramer() {
}

Boolean H264VideoStreamFramer::isH264VideoStreamFramer() const {
  return True;
}

void H264VideoStreamFramer::setSPSandPPS(char const* sPropParameterSetsStr) {
  unsigned numSPropRecords;
  SPropRecord* sPropRecords = parseSPropParameterSets(sPropParameterSetsStr, numSPropRecords);
  for (unsigned i = 0; i < numSPropRecords; ++i) {
    if (sPropRecords[i].sPropLength == 0) continue; // bad data
    u_int8_t nal_unit_type = (sPropRecords[i].sPropBytes[0])&0x1F;
    if (nal_unit_type == 7/*SPS*/) {
      saveCopyOfSPS(sPropRecords[i].sPropBytes, sPropRecords[i].sPropLength);
    } else if (nal_unit_type == 8/*PPS*/) {
      saveCopyOfPPS(sPropRecords[i].sPropBytes, sPropRecords[i].sPropLength);
    }
  }
  delete[] sPropRecords;
}
