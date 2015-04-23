/*
 * Copyright 2015 Jason Downing
 * All rights reserved.
 * MIT Licensed - see http://opensource.org/licenses/MIT for details.
 *
 */
#include <boost/regex.hpp>
#include <iostream>
#include <fstream>
#include <string>


int main(int argc, const char* argv[]) {
  // Make sure the user knows how to use our amazing log parser.
  if (argc != 2) {
    std::cout << "./ps7a device1_intouch.log\n";
    return 0;
  }

  // Counters
  int lines_scan = 1;     // START AT LINE 1 - not 0. opps.
  int boot_success = 0;
  int boot_total = 0;

  // Some strings we need. File names, other stuff.
  std::string file_name(argv[1]);
  std::string output_name = file_name + ".rpt";
  std::string report = "";
  std::string boots = "";

  // Begin time / End time strings.
  std::string begin_time = "";
  std::string end_time = "";
  std::string time_difference = "1234";

  // Need to match against something like this:
  // Start of boot: 2014-02-01 14:02:32: (log.c.166) server started
  // Success if we find:
  // "2014-01-26 09:58:04.362:INFO:oejs.AbstractConnector:Started
  // SelectChannelConnector@0.0.0.0:9080"
  std::string start_string = "([0-9]{4})-([0-9]{2})-([0-9]{2}) ";
  start_string += "([0-9]{2}):([0-9]{2}):([0-9]{2}): \\(log.c.166\\) ";
  start_string += "server started";
  std::string end_string = "([0-9]{4})-([0-9]{2})-([0-9]{2}) ";
  end_string += "([0-9]{2}):([0-9]{2}):([0-9]{2}).([0-9]{3}):INFO:oejs.";
  end_string += "AbstractConnector:Started SelectChannelConnector@0.0.0.0:9080";

  // Testing.
  //std::cout << "Start string is: \n" << start_string << "\n";
  //std::cout << "2014-02-01 14:02:32: (log.c.166) server started\n";

  // ("\\(([^)]+)\\)\"$", boost::regex::perl)

  // Make two regexes
  boost::regex start_regex(start_string, boost::regex::perl);
  boost::regex end_regex(end_string);

  // Use this for getting parts of the matched string.
  boost::smatch sm;

  report += "Device boot report.\n\n";
  report += "InTouch log file: " + file_name + "\n";

  // Read the file here and do stuff.
  // Save stuff to a string with formatting to output later on.
  std::string line;
  std::ifstream file (file_name.c_str());

  // Need to keep track of when we've found a start.
  bool found_start = false;

  if (file.is_open()) {
    while (getline(file, line)) {
      // We've got the current string here and can do stuff with it.

      // Wipe the begin_time / end_time strings
      begin_time = "";
      end_time = "";

      // Let's try and see if we found a start boot.
      if (boost::regex_search (line, sm, start_regex)) {
        // Get the start time, save it for later.
        // Note: sm[0] is the ENTIRE match. We just want the date.
        begin_time += sm[1] + "-" + sm[2] + "-" + sm[3];
        begin_time += " " + sm[4] + ":" + sm[5] + ":" + sm[6];

        // We can use this begin_time for calculations later on.

        // If we already found a start, then this is an incomplete boot.
        if (found_start == true) {
          boots += "**** Incomplete boot ****\n\n";
        }

        // Now we want to add this to the output string as boot start.
        boots += "=== Device boot ===\n";
        boots += std::to_string(lines_scan) + "(" + file_name + ") ";
        boots += begin_time + " Boot Start\n";

        boot_total++;

        // Match this as a "found_start"
        found_start = true;
      }

      // Or did we find a successful boot?
       if (boost::regex_match (line, sm, end_regex)) {
        // Get the end time, save it for later.
        end_time += sm[1] + "-" + sm[2] + "-" + sm[3];
        end_time += " " + sm[4] + ":" + sm[5] + ":" + sm[6];

        // Add the end boot line and total time it took to get here.
        boots += std::to_string(lines_scan) + "(" + file_name + ") ";
        boots += end_time + " Boot Completed\n";

        // Do some magic here and calculate the time it took to boot in ms
        // Use the begin_time and the end_time variables.

        

        // Now add the time difference.
        boots += "\tBoot Time: " + time_difference + "ms\n\n";

        boot_success++;

        // Match found_start as false for the next loop.
        found_start = false;
       }

      lines_scan++;
    }
    file.close();
  }

  // Add lines scanned to the report.
  report += "Lines scanned: " + std::to_string(lines_scan) + "\n\n";

  // We can figure out success boots and fail boots now.
  report += "Device boot count: initiated = " + std::to_string(boot_total);
  report += ", completed: " + std::to_string(boot_success) + "\n\n\n";

  // We should now add the boot reports to the end of this report.
  report += boots;

  // Now let's print out our report string. It has already been formatted
  // correctly so it should be fairly simple to just dump it to stdio.
  std::cout << report;

  // And we can even save this to a file with the extension ".rpt"
  // which would be something like "device5_intouch.log.rpt"
  // or filename + ".rpt"

  return 0;
}
