//
//  gps_trajectory_reader_seattle_sample.cpp
//  press-v3
//
//  Created by Renchu Song on 9/7/20.
//  Copyright © 2020 Renc. All rights reserved.
//

#include "gps_trajectory_reader_seattle_sample.hpp"

#include <cassert>

int getMonth(char* monthChar) {
  std::string monStr(monthChar, 3);
  if (monStr == "Jan") {
    return 0;
  }
  if (monStr == "Feb") {
    return 1;
  }
  if (monStr == "Mar") {
    return 2;
  }
  if (monStr == "Apr") {
    return 3;
  }
  if (monStr == "May") {
    return 4;
  }
  if (monStr == "Jun") {
    return 5;
  }
  if (monStr == "Jul") {
    return 6;
  }
  if (monStr == "Aug") {
    return 7;
  }
  if (monStr == "Sep") {
    return 8;
  }
  if (monStr == "Oct") {
    return 9;
  }
  if (monStr == "Nov") {
    return 10;
  }
  if (monStr == "Dec") {
    return 11;
  }
  assert(false);
}

void GPSTrajectoryReaderSeattleSample::readGPSTrajectoryImpl(
  const std::string& fileName,
   GPSTrajectory& gpsTrajectory
) {
  std::vector<GPSPoint> gpsPoints;
  
  FileReader reader(fileName.c_str(), false);
  
  // Skip the header line.
  do { } while (reader.nextChar() != '\n');
  
  while (true) {
    int date = reader.nextInt();
    if (date == EOF) {
      break;
    }
    reader.nextChar();
    int mon = getMonth(reader.nextString(3));
    reader.nextChar();
    int year = reader.nextInt();
    int hour = reader.nextInt();
    reader.nextChar();
    int min = reader.nextInt();
    reader.nextChar();
    int sec = reader.nextInt();
    struct tm t = {0};  // Initalize to all 0's
    t.tm_year = year - 1900;
    t.tm_mon = mon;
    t.tm_mday = date;
    t.tm_hour = hour;
    t.tm_min = min;
    t.tm_sec = sec;
    time_t timeSinceEpoch = mktime(&t);

    gpsPoints.emplace_back(GPSPoint(timeSinceEpoch, reader.nextDouble(), reader.nextDouble()));
  }
  
  gpsTrajectory.setGPSTrajectory(gpsPoints);
}

