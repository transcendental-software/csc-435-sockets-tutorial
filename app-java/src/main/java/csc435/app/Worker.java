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
                if (inputLine.compareTo("quit") == 0) {
                    break;
                }

                if (inputLine.compareTo("addition") == 0) {
                    // send reply message to the client
                    String outputLine = "2+2=4";
                    out.println(outputLine);
                    continue;
                }

                if (inputLine.compareTo("multiplication") == 0) {
                    // send reply message to the client
                    String outputLine = "2x2=4";
                    out.println(outputLine);
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
