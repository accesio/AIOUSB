
import com.jdotsoft.jarloader.JarClassLoader;

public class MainTestApp {

    public static void main(String[] args) {
        JarClassLoader jcl = new JarClassLoader();
        System.out.println("Starting TestConsole...");
        try {
            jcl.invokeMain("test", args);
        } catch (Throwable e) {
            e.printStackTrace();
        }
    }
    
}
