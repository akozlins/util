
public class Test
{
  public static native int sum(int a, int b);

  public static void main(String ... args)
  {
    System.loadLibrary("Test");
    System.out.println("sum(1, 2) = " + sum(1, 2));
  }
}
