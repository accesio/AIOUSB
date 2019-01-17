
package com.accesio;

import com.accesio.read_channels_test_java;
import com.jdotsoft.jarloader.JarClassLoader;



public class MyAppLauncher {

    public static void main(String[] args) {
        JarClassLoader jcl = new JarClassLoader();
        try {
            jcl.invokeMain("com.accesio.read_channels_test_java", args);
        } catch (Throwable e) {
            e.printStackTrace();
        }
    } // main()

} // class MyAppLauncher
