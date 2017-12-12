public class  VprocessJNI{

	public VprocessJNI(){}
	public long inst = 0;
	static{
		System.loadLibrary("libprocess");
	}

	public native long create_obj(int size,int fs,int fsize,int move);
	public native void process_feat(long inst,double[] data);
	public native double[] compare(long inst1,long inst2);
	public native void destroy(long inst);

}
