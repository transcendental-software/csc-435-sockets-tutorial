package csc435.app;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;

public class Worker implements Runnable {
    private Socket socket;

    public Worker(Socket socket) {
        this.socket = socket;
    }

    @Override
    public void run() {
        try{
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            String inputLine;
            
            // receive a request message from a client
            while ((inputLine = in.readLine()) != null) {
                if (inputLine.compareTo("QUIT") == 0) {
                    break;
                }

                if (inputLine.substring(0, 5).compareTo("INDEX") == 0) {
                    String[] tokens = inputLine.split("\\s+");

                    System.out.println("\nindexing " + tokens[2] + " from " + tokens[1]);
                    for (int i = 3; i < tokens.length; i+=2) {
                        System.out.println(tokens[i] + " " + tokens[i + 1]);
                    }
                    System.out.println("completed!");
                    
                    // send reply message to the client
                    String outputLine = "OK";
                    out.println(outputLine);

                    System.out.print("> ");
                    System.out.flush();
                    continue;
                }

                if (inputLine.substring(0, 6).compareTo("SEARCH") == 0) {
                    String[] tokens = inputLine.split("\\s+");

                    System.out.println("\nsearching for " + tokens[0]);
                    
                    // send reply message to the client
                    String outputLine = "DOC10 20 DOC100 30 DOC1 10";
                    out.println(outputLine);

                    System.out.print("> ");
                    System.out.flush();

                    continue;
                }
                
                // send reply message to the client
                String outputLine = "???";
                out.println(outputLine);
            }
        } catch (IOException e) {
            System.out.println("Socket IO error!");
            e.printStackTrace();
        }
    }
}
