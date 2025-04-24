package Offline_1;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;

public class NetworkUtil {
    private Socket socket;
    private ObjectOutputStream oos;
    private ObjectInputStream ois;

    public NetworkUtil(String s , int port) throws IOException {
        socket = new Socket(s , port);
        oos = new ObjectOutputStream(socket.getOutputStream());
        ois = new ObjectInputStream(socket.getInputStream());
    }

    public NetworkUtil(Socket s) throws IOException{
        socket = s;
        oos = new ObjectOutputStream(socket.getOutputStream());
        ois = new ObjectInputStream(socket.getInputStream());
    }

    public Object read() throws IOException, ClassNotFoundException {
        return ois.readUnshared();
    }

    public int readByte(byte[] bytes) throws IOException {
        return ois.read(bytes);
    }

    public void write(Object o) throws IOException {
        oos.writeUnshared(o);
    }

    public void writeInVast(byte[] o , int a , int b) throws IOException {
        oos.write(o , a , b);
    }

    public void flush() throws IOException {
        oos.flush();
    }


    public void closeConnection() throws IOException {
        ois.close();
        oos.close();
    }

    public Socket getSocket(){
        return this.socket;
    }
}
