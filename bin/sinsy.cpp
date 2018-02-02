/* ----------------------------------------------------------------- */
/*           The HMM-Based Singing Voice Synthesis System "Sinsy"    */
/*           developed by Sinsy Working Group                        */
/*           http://sinsy.sourceforge.net/                           */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2009-2015  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the Sinsy working group nor the names of    */
/*   its contributors may be used to endorse or promote products     */
/*   derived from this software without specific prior written       */
/*   permission.                                                     */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */

#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "sinsy.h"

namespace
{
const char* DEFAULT_LANGS = "j";
};

void usage()
{
   std::cout << "The HMM-Based Singing Voice Syntheis System Remix \"Sinsy-R\"" << std::endl;
   std::cout << "Version 0.95 (http://sinsy.sourceforge.net/)" << std::endl;
   std::cout << "Copyright (C) 2017-2018 HyperDeep (a.k.a hyperzlib / Quantum)" << std::endl;
   std::cout << "              2009-2017 Nagoya Institute of Technology" << std::endl;
   std::cout << "All rights reserved." << std::endl;
   std::cout << "" << std::endl;
   std::cout << "The HMM-Based Speech Synthesis Engine \"hts_engine API\"" << std::endl;
   std::cout << "Version 1.10 (http://hts-engine.sourceforge.net/)" << std::endl;
   std::cout << "Copyright (C) 2001-2015 Nagoya Institute of Technology" << std::endl;
   std::cout << "              2001-2008 Tokyo Institute of Technology" << std::endl;
   std::cout << "All rights reserved." << std::endl;
   std::cout << "" << std::endl;
   std::cout << "  usage:" << std::endl;
   std::cout << "    sinsy [ options ] [ infile ]" << std::endl;
   std::cout << "  options:                                           [def]" << std::endl;
   std::cout << "    -w langs    : languages                          [  j]" << std::endl;
   std::cout << "                  j: Japanese                             " << std::endl;
   std::cout << "                  c: Chinese                              " << std::endl;
   std::cout << "    -x dir      : dictionary directory               [/usr/local/dic]" << std::endl;
   std::cout << "    -m htsvoice : HTS voice file                     [N/A]" << std::endl;
   std::cout << "    -o file     : filename of output wav audio       [N/A]" << std::endl;
   std::cout << "    -s time     : play start time                    [0.0]" << std::endl;
   std::cout << "    -l mode     : output label                       [  d]" << std::endl;
   std::cout << "                  d: Disable                              " << std::endl;
   std::cout << "                  n: Normal                               " << std::endl;
   std::cout << "                  t: Label with time                      " << std::endl;
   std::cout << "                  m: Mono label                           " << std::endl;
   std::cout << "  infile:" << std::endl;
   std::cout << "    MusicXML file" << std::endl;
}

int main(int argc, char **argv)
{
   if (argc < 2) {
      usage();
      return -1;
   }

   std::string xml;
   std::string voice;
   std::string config("/usr/local/dic");
   std::string wav;
   std::string startTime;
   std::string languages(DEFAULT_LANGS);
   int outputLabel = 0;

   int i(1);
   for(; i < argc; ++i) {
      if ('-' != argv[i][0]) {
         if (xml.empty()) {
            xml = argv[i];
         } else {
            std::cout << "[ERROR] invalid option : '" << argv[i][1] << "'" << std::endl;
            usage();
            return -1;
         }
      } else {
         switch (argv[i][1]) {
         case 'w' :
            languages = argv[++i];
            break;
         case 'x' :
            config = argv[++i];
            break;
         case 'm' :
            voice = argv[++i];
            break;
         case 'o' :
            wav = argv[++i];
            break;
		 case 's' :
			startTime = argv[++i];
			break;
         case 'h' :
            usage();
            return 0;
         case 'l' :
			switch(argv[++i][0]){
				case 'd':
					outputLabel = 0;
					break;
				case 'n':
					outputLabel = 1;
					break;
				case 't':
					outputLabel = 2;
					break;
				case 'm':
					outputLabel = 3;
					break;
				default:
					outputLabel = 0;
					break;
			}
            break;
         default :
            std::cout << "[ERROR] invalid option : '-" << argv[i][1] << "'" << std::endl;
            usage();
            return -1;
         }
      }
   }

   if(xml.empty() || voice.empty()) {
      usage();
      return -1;
   }

   sinsy::Sinsy sinsy;

   std::vector<std::string> voices;
   voices.push_back(voice);

   if (!sinsy.setLanguages(languages, config)) {
      std::cout << "[ERROR] failed to set languages : " << languages << ", config dir : " << config << std::endl;
      return -1;
   }

   if (!sinsy.loadVoices(voices)) {
      std::cout << "[ERROR] failed to load voices : " << voice << std::endl;
      return -1;
   }

   if (!sinsy.loadScoreFromMusicXML(xml)) {
      std::cout << "[ERROR] failed to load score from MusicXML file : " << xml << std::endl;
      return -1;
   }

   sinsy::SynthCondition condition;

   if (wav.empty()) {
      condition.setPlayFlag();
   } else {
      condition.setSaveFilePath(wav);
   }
   
   if (outputLabel > 0) {
      condition.setOutputLabel();
	  if(outputLabel == 2){
		sinsy.outputLabel(true);
	  }
	  if(outputLabel == 3){
		sinsy.outputMonoLabel(true);
	  }
   } else {
      condition.unsetOutputLabel();
	  sinsy.outputLabel(false);
   }
   
   if (!startTime.empty()){
	   sinsy.setStartTime(startTime);
   }

   sinsy.synthesize(condition);

   return 0;
}
