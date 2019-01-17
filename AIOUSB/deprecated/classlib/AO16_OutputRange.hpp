/*
 * $RCSfile: AO16_OutputRange.hpp,v $
 * $Date: 2009/12/19 00:27:36 $
 * $Revision: 1.1 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class AO16_OutputRange declarations
 */

#if ! defined( AO16_OutputRange_hpp )
#define AO16_OutputRange_hpp


#include <AnalogIORange.hpp>


namespace AIOUSB {

class AO16_OutputRange : public AnalogIORange {
	friend class AO16_AnalogOutputSubsystem;

protected:
	AO16_OutputRange();
	AO16_OutputRange( int minCounts, int maxCounts );
	virtual ~AO16_OutputRange();

public:
	virtual AnalogIORange &setRange( int range );
};	// class AO16_OutputRange

}	// namespace AIOUSB

#endif

/* end of file */
