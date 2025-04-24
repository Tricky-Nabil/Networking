package Offline_1;

import java.io.*;
import java.net.SocketTimeoutException;
import java.util.Scanner;

public class WorkerClient{
    private NetworkUtil networkUtil;
    public String clientName;
    public
    WorkerClient(NetworkUtil n , String str){
        networkUtil = n;
        clientName = str;
        run();
    }
    public void run() {
        while(true){
            System.out.println("Choose an option --- ");
            System.out.println("1. Look up the Clients who were connected at least one time");
            System.out.println("2. Look up files uploaded by you");
            System.out.println("3. Look up the public files of other clients");
            System.out.println("4. Make a file request to other clients");
            System.out.println("5. View unread message of yours");
            System.out.println("6. Upload a file");
            System.out.println("7. Upload a file requested by other client");
            System.out.println("8. Log out");

            Scanner scn = new Scanner(System.in);
            String input = scn.nextLine();
            if(input.equalsIgnoreCase("1")){
                try{
                    networkUtil.write("No_1"); // See the client list
                    var str = (String)networkUtil.read();
                    //System.out.println(str);
                    while(!str.equalsIgnoreCase("NO_MORE_CLIENTS_LEFT")){
                        System.out.println(str);
                        str = (String)networkUtil.read();
                    }

                }catch(Exception e){}

            }
            else if(input.equalsIgnoreCase("2")){
                try{
                    networkUtil.write("No_2");
                    var str = (String)networkUtil.read();
                    while(!str.equalsIgnoreCase("NO_MORE_FILES_LEFT")){
                        System.out.println(str);
                        str = (String)networkUtil.read();
                    }

                    var wanna_download_files = (String)networkUtil.read();
                    System.out.println(wanna_download_files);
                    System.out.println("Select an option\n1.Yes\n2.No");
                    String download_status = scn.nextLine();
                    networkUtil.write(download_status);
                    if(download_status.equalsIgnoreCase("NO"))
                        continue;

                    var how_many_files_wanna_download = (String)networkUtil.read();
                    System.out.println(how_many_files_wanna_download);
                    String file_number_to_download = scn.nextLine();
                    Integer file_number_To_Download = Integer.parseInt(file_number_to_download);
                    networkUtil.write(file_number_To_Download);

                    for(int i = 0 ; i < file_number_To_Download ; i++){
                        System.out.println("Enter the valid fileID : ");
                        String file_id = scn.nextLine();
                        Integer File_Id = Integer.parseInt(file_id);
                        networkUtil.write(File_Id);
                        Boolean isRightFileId = (Boolean)networkUtil.read();
                        if(isRightFileId){
                            client_receiveFile();
                        }
                        else{
                            System.out.println("Wrong input given");
                        }
                    }

                }catch(Exception e){}
            }
            else if(input.equalsIgnoreCase("3")){
                try {
                    networkUtil.write("NO_3");
                    var public_files_of_other_clients = (String)networkUtil.read();
                    while(!public_files_of_other_clients.equalsIgnoreCase("NO_MORE_PUBLIC_FILES_LEFT")){
                        System.out.println(public_files_of_other_clients);
                        public_files_of_other_clients = (String) networkUtil.read();
                    }
                    var wanna_download_files = (String)networkUtil.read();
                    System.out.println(wanna_download_files);
                    System.out.println("Select an option\n1.Yes\n2.No");
                    var download_status = scn.nextLine();
                    networkUtil.write(download_status);
                    if(download_status.equalsIgnoreCase("NO"))
                        continue;

                    var how_many_files_wanna_download = (String)networkUtil.read();
                    System.out.println(how_many_files_wanna_download);
                    String file_number_to_download = scn.nextLine();
                    Integer file_number_To_Download = Integer.parseInt(file_number_to_download);
                    networkUtil.write(file_number_To_Download);

                    for(int i = 0 ; i < file_number_To_Download ; i++){
                        System.out.println("Enter the valid fileID : ");
                        String file_id = scn.nextLine();
                        Integer File_Id = Integer.parseInt(file_id);
                        networkUtil.write(File_Id);
                        Boolean isRightFileId = (Boolean)networkUtil.read();
                        if(isRightFileId){
                            client_receiveFile();
                        }
                        else{
                            System.out.println("Wrong input given");
                        }
                    }

                } catch (Exception e) {
                    e.printStackTrace();
                }

            }
            else if(input.equalsIgnoreCase("4")){
                try {
                    networkUtil.write("NO_4");
                    var give_file_description = (String)networkUtil.read();
                    System.out.println(give_file_description);
                    System.out.println("Enter Description : ");
                    String description = scn.nextLine();
                    networkUtil.write(description);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            else if(input.equalsIgnoreCase("5")){
                try{
                    networkUtil.write("NO_5");
                    var unread_msg = (String)networkUtil.read();
                    while(!unread_msg.equalsIgnoreCase("NO_UNREAD_MESSAGE_LEFT")){
                        System.out.println(unread_msg);
                        unread_msg = (String) networkUtil.read();
                    }
                }catch(Exception e){

                }
            }
            else if(input.equalsIgnoreCase("6")){
                try {
                    networkUtil.write("NO_6");
                    var give_file_name = (String)networkUtil.read();
                    System.out.println(give_file_name);
                    String file_name = scn.nextLine();
                    String file_path = "src/clientDirectories/" + file_name;
                    File file = new File(file_path);

                    if(!file.exists()){
                        System.out.println("Wrong File Name Given");
                        networkUtil.write("Wrong");
                        continue;
                    }
                    networkUtil.write("Right");
                    networkUtil.write(file_name);
                    var private_or_public = (String)networkUtil.read();
                    System.out.println(private_or_public);
                    System.out.println("Select a option\n1.Private\n2.Public");
                    String file_type = scn.nextLine();
                    networkUtil.write(file_type);

                    client_sendFile(file , file_path);


                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            else if(input.equalsIgnoreCase("7")){
                try {
                    networkUtil.write("NO_7");
                    var give_request_id = (String)networkUtil.read();
                    System.out.println(give_request_id);
                    String reqId = scn.nextLine();
                    networkUtil.write(Integer.parseInt(reqId));

                    Boolean isValidReqId = (Boolean)networkUtil.read();
                    if(!isValidReqId){
                        System.out.println("Sorry. Not a valid request ID");
                        continue;
                    }
                    var give_file_name = (String)networkUtil.read();
                    System.out.println(give_file_name);
                    String file_name = scn.nextLine();
                    String file_path = "src/clientDirectories/" + file_name;
                    File file = new File(file_path);

                    if(!file.exists()){
                        System.out.println("Wrong File Name Given");
                        networkUtil.write("Wrong");
                        continue;
                    }
                    networkUtil.write("Right");
                    networkUtil.write(file_name);

                    //System.out.println("Select a option\n1.Private\n2.Public");
//                    String file_type = scn.nextLine();
//                    networkUtil.write(file_type);

                    client_sendFile(file , file_path);

                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            else if(input.equalsIgnoreCase("8")){
                try {
                    networkUtil.write("NO_8");
                    var logOutMsg = (String)networkUtil.read();
                    System.out.println(logOutMsg);
                    System.out.println("Logging Out " + clientName);
                    networkUtil.getSocket().close();
                    break;
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }
    private void client_receiveFile(){
        String file_name = null;
        try {
            file_name = (String) networkUtil.read();
            int chunkSize = 1024;
            int chunkNumber = 1;
            byte[] buffer = new byte[chunkSize];
            int bytesRead;
            System.out.println(" Download Starts ....");
            String file_path = "src/ClientDownloads/" + clientName;
            File directory = new File(file_path);
            directory.mkdirs();
            file_path += ("/"+file_name);
            FileOutputStream fileOutputStream = new FileOutputStream(file_path);
            BufferedOutputStream bufferedOutputStream = new BufferedOutputStream(new FileOutputStream(file_path));
            int total_Bytes_Read = 0;
            while ((bytesRead = networkUtil.readByte(buffer)) != -1) {
                total_Bytes_Read += bytesRead;
                bufferedOutputStream.write(buffer, 0, bytesRead);
                bufferedOutputStream.flush();
                System.out.println(chunkNumber + " no chunk received from client side\n " + bytesRead + " bytes received");
                chunkNumber++;
            }
            String completion_msg_from_server = (String) networkUtil.read();
            System.out.println(completion_msg_from_server);
            System.out.println("File downloaded successfully!! \nTotal " + total_Bytes_Read + " bytes received");
            bufferedOutputStream.close();

        } catch (IOException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }


    }
    private void client_sendFile(File file, String file_path){
        boolean is_upload_done = true;
        try{
            //is_upload_done = true;
            String give_file_size = (String) networkUtil.read();
            System.out.println(give_file_size);
            long fileSize = file.length();
            networkUtil.write(fileSize);

            String buffer_status = (String) networkUtil.read();
            if(buffer_status.equalsIgnoreCase("BUFFER_SIZE_OVERFLOWED")) {
                System.out.println("Buffer size overflowed. You cannot upload file now");
                return;
            }
            int chunkSize = (int)networkUtil.read();
            System.out.println("Server gives the chunk size of : " + chunkSize + " bytes");
            FileInputStream fileInputStream = new FileInputStream(new File(file_path));
            BufferedInputStream bufferedInputStream = new BufferedInputStream(fileInputStream);
            byte[] chunk = new byte[chunkSize];
            int bytesRead;
            int chunkNumber = 1;

            while ((bytesRead = bufferedInputStream.read(chunk)) != -1){
                System.out.println(bytesRead + " bytes read");
                networkUtil.writeInVast(chunk , 0 , bytesRead);
                networkUtil.flush();

                while (!((String)networkUtil.read()).equalsIgnoreCase("CHUNKS_RECEIVED")){

                }
                System.out.println(chunkNumber + " no chunk is sent successfully!!");
                chunkNumber++;

            }

        }
        catch(SocketTimeoutException e){
            System.out.println("TIMEOUT!Server did not acknowldge chunk within 30 seconds..UPLOAD FAILED");
            is_upload_done = false;
        }
        catch (IOException e){
            System.out.println(e);
        }
        catch(ClassNotFoundException e){
            System.out.println(e);
            boolean type = true;
        }
        if(is_upload_done){
            try {
                networkUtil.write("FINISHED");
                Boolean server_status = (Boolean) networkUtil.read();
                if(server_status){
                    System.out.println("File sent successfully!!");
                }else{
                    System.out.println("File not sent!!");
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }else{
            try {
                networkUtil.write("NOT_FINISHED");
            } catch (IOException e) {
                e.printStackTrace();
            }
            System.out.println("Time Out!!");
        }

    }
}
