/*
 * $RCSfile: DeviceSubsystem.java,v $
 * $Date: 2009/12/16 23:02:15 $
 * $Revision: 1.5 $
 * jEdit:tabSize=4:indentSize=4:collapseFolds=1:
 */

package com.acces.aiousb;

// {{{ imports
import java.io.*;
// }}}

/**
 * Class DeviceSubsystem is the abstract super class for all device subsystems.
 */

abstract class DeviceSubsystem {

	// {{{ protected members
	protected USBDevice parent;					// parent device that this subsystem is part of
	// }}}

	// {{{ protected methods

	protected DeviceSubsystem( USBDevice parent ) {
		this.parent = parent;
	}	// DeviceSubsystem()

	/*
	 * properties
	 */

	public abstract PrintStream print( PrintStream stream );

	/**
	 * Gets the parent device that this subsystem is part of.
	 * @return The parent device that this subsystem is part of.
	 */

	public USBDevice getParent() {
		return parent;
	}	// getParent()

	protected int getDeviceIndex() {
		assert parent != null;
		return parent.deviceIndex;
	}	// getDeviceIndex()

	// }}}

}	// class DeviceSubsystem

/* end of file */
