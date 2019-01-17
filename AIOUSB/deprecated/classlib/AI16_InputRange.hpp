/*
 * $RCSfile: AI16_InputRange.hpp,v $
 * $Date: 2009/12/19 00:27:36 $
 * $Revision: 1.1 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 *
 * class AI16_InputRange declarations
 */

#if ! defined( AI16_InputRange_hpp )
#define AI16_InputRange_hpp


#include <AnalogIORange.hpp>


namespace AIOUSB {

class AI16_InputRange : public AnalogIORange {
	friend class AnalogInputSubsystem;
	friend class AI16_DataPoint;

protected:
	AI16_InputRange();
	AI16_InputRange( int minCounts, int maxCounts );
	virtual ~AI16_InputRange();

public:
	virtual AnalogIORange &setRange( int range );
};	// class AI16_InputRange

}	// namespace AIOUSB

#endif

/* end of file */
