package Offline_1;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Scanner;

public class Client {
    //public String clientName = "";
    public Client(String serverAddress, int serverPort) {

        try {
            System.out.print("Enter name of the client: ");
            Scanner scanner = new Scanner(System.in);
            String clientName = scanner.nextLine();
            //clientName = scanner.nextLine();
            NetworkUtil networkUtil = new NetworkUtil(serverAddress, serverPort);
            networkUtil.write(clientName);
            String log_in_success = (String)networkUtil.read();
            if(log_in_success.equalsIgnoreCase("Successful")){
                System.out.println("Connection Established");
                System.out.println("Remort Port: " + networkUtil.getSocket().getPort());
                System.out.println("Local Port: " + networkUtil.getSocket().getLocalPort());
                new WorkerClient(networkUtil , clientName);
            }
            else if(log_in_success.equalsIgnoreCase("Unsuccessful")) {
                System.out.println("Sorry. User name already exists");
                networkUtil.getSocket().close();
                //networkUtil.closeConnection();
                return;
            }
        } catch (Exception e) {
            System.out.println(e);
        }
    }

    public static void main(String args[]) {
        String serverAddress = "127.0.0.1";
        int serverPort = 33333;
        Client client = new Client(serverAddress, serverPort);
    }
}
