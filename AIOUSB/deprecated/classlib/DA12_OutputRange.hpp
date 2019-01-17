/*
 * $RCSfile: DA12_OutputRange.hpp,v $
 * $Date: 2009/12/19 00:27:36 $
 * $Revision: 1.1 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class DA12_OutputRange declarations
 */

#if ! defined( DA12_OutputRange_hpp )
#define DA12_OutputRange_hpp


#include <AnalogIORange.hpp>


namespace AIOUSB {

class DA12_OutputRange : public AnalogIORange {
	friend class DA12_AnalogOutputSubsystem;

protected:
	DA12_OutputRange();
	DA12_OutputRange( int minCounts, int maxCounts );
	virtual ~DA12_OutputRange();

public:
	virtual AnalogIORange &setRange( int range );
};	// class DA12_OutputRange

}	// namespace AIOUSB

#endif

/* end of file */
