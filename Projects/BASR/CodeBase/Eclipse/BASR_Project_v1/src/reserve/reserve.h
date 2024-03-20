/*
 * reserve.h
 *
 *  Created on: Oct 15, 2018
 *      Author: test
 */

#ifndef RESERVE_H_
#define RESERVE_H_

#include "../common/common.h"
#include <tuple>
#include <stdio.h>
#include <time.h>
#include "../delegate/analysisangledelegate.h"

//notes for low application
//emitters must switch off updates after 10ms or a certain multiple of their last pri
//emitters whose updates are off can be strung with active emitters (discuss extent of dsp stringing)
//emitters whose updates are off must be reported before being deleted
//operator overloading in track pri could cause problems

//notes for high application
//where to store pri/pw/rf for last report as well as accumulative? as match would only be done on accumulative

//old notes
//copy mat data from io variables into a list, determine locations done
//find suitable location for freeing io variables done
//see if open function definition is available in cdt done
//fix simulation timer at variable interval done
//check expiry of accumulated pdw done
//check rollover count of pdw with iter: 1 ms done
//implement flag to skip a time window in case of penalty done

//some constants need to be moved back to model if they are actually variables
//review analysislibrarydelegate and libraryclass architecture and names
//need a new input pdw list cleaning algorithm
//multiple input types other than mat not supported or available for selection
//how to manage thread safety in this application?
//how to divide pdw amongst various bands in this application?
//keep some overlap region amongst input pdw for tracking purposes
//find a more portable version of matio.a for x86
//finalize internal pri/pw/rf/attitude etc structures
//where to incorporate control panel and warning lights function and logic?
//knowledge from old dsp and cpu builds must be assimilated
//we may need a logging/debugging class for testing purposes
//how to make the code multi-core friendly by limiting access of global state


//decide eventually when to serialize/deserialize data to other processor
//..including task division`
//where would antenna switching feature in this application?
//clustering notes
//..top down vs bottom up clustering techniques can be used
//..some general rules for how much a radar can vary parameters need r&d
//..would we run multiple clustering algorithms for parameters separately?
//..or would we combine all of them in one algorithm? we can also cluster pre
//.. sorted sets of pdw based on rf, pw, aoa, intrapulse, candidate pri and
//..intersect results based on common pdws
//..we can run multiple clustering algos in parallel depending on situation
//tracking notes
//..determine trail of pulse that would go through tracking, clustering and
//..and maybe being discarded. How long would we retain pulses for future
//..iterations?
//..Some Tracking criteria:
//..angle and exact pw of last few pulses (w/rf exclusion)
//..angle and power, as well as catering to scan shape (w/rf exclusion)
//..angle and pri tracking (w/rf exclusion)
//..angle and exact rf
//..after sinking a pulse, update the last running rf, pw, difftoa etc provided
//..the tracking "score" of the sunk pulse justifies this updation, there maybe
//..some weak scored pulses and using them for parameter calculation is best
//..avoided, each PDW sunk into a track might need a "score" parameter
//pri de-interleaving will only work with simple diff TOAs, since complex
//types cannot be surely determined without first de-interleaving pulses

//determine how toa, power, angle, pw, rf scaling would be done during
//processing as well as afterwards when converting to micros (us) for other
//processes, determine how many constants would be required throughout



//All definitions
#ifdef RESERVE_CPP_
//All file scope variables
#else
//Global variables using extern
#endif
//End of conditional exclusion

#endif /* RESERVE_H_ */
