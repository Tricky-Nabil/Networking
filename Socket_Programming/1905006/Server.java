package Offline_1;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;

public class Server {

    private ServerSocket serverSocket;

    public static final int MIN_CHUNK_SIZE = 100;
    public static final int MAX_CHUNK_SIZE = 200;
    public static final int MAX_BUFFER_SIZE = 80000;
    public static volatile int used_buffer_size = 0;
    public static volatile int fileID = 1;
    public static volatile int reqID = 1;

    public static volatile HashMap<String, NetworkUtil> clientMap = new HashMap<>();
    public static volatile HashMap<String,ClientInformation> clientInformationMap = new HashMap<>();
    public static volatile HashMap<Integer,Request> requestMap = new HashMap<>();
    public static volatile HashMap<String,Message> unreadMessagesMap = new HashMap<>();
    public static volatile HashMap<Integer,FileDescription> fileMap = new HashMap<Integer, FileDescription>();

    Server() {
        try {
            serverSocket = new ServerSocket(33333);
            while (true) {
                Socket clientSocket = serverSocket.accept();
                serve(clientSocket);
            }
        } catch (Exception e) {
            System.out.println("Server starts:" + e);
        }
    }

    public void serve(Socket clientSocket) throws IOException, ClassNotFoundException {
        NetworkUtil networkUtil = new NetworkUtil(clientSocket);
        String clientName = (String) networkUtil.read();
        new WorkerServer(networkUtil , clientName);
//        if(clientMap.get(clientName) == null){
//            clientMap.put(clientName , networkUtil);
//            networkUtil.write("Successful");
//            System.out.println("Client " + clientName + " has successfully logged in");
//            new WorkerServer(networkUtil , clientName);
//        }
//        else{
//            System.out.println("User name already exist");
//        }

        //new ReadThreadServer(clientMap, networkUtil);
    }

    public static void main(String args[]) {
        Server server = new Server();
    }
}
