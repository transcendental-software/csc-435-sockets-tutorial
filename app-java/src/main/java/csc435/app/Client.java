package csc435.app;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

public class Client {
    private String address;
    private Integer port;

    public Client(String address, int port) {
        this.address = address;
        this.port = port;
    }

    public void run() {
        try {
            // create client socket and connect to server
            Socket socket = new Socket(InetAddress.getByName(address), port);
            
            // extract server information
            System.out.println("Client connected to " + address);

            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            String outputLine;
            String inputLine;

            // send request message to server (worker)
            outputLine = "addition";
            out.println(outputLine);
            // receive reply message from server (worker)
            inputLine = in.readLine();
            System.out.println(inputLine);
            
            // send request message to server (worker)
            outputLine = "multiplication";
            out.println(outputLine);
            // receive reply message from server (worker)
            inputLine = in.readLine();
            System.out.println(inputLine);
            
            // send request message to server (worker)
            outputLine = "quit";
            out.println(outputLine);
            
            // close connection
            socket.close();
        } catch (UnknownHostException e) {
            System.err.println("Could not compute IP address!");
            e.printStackTrace();
        } catch (IOException e) {
            System.err.println("Socket error!");
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        if (args.length != 2) {
            System.err.println("USE: java Client <IP address> <port>");
            System.exit(1);
        }

        Client client = new Client(args[0], Integer.parseInt(args[1]));
        client.run();
    }
}
