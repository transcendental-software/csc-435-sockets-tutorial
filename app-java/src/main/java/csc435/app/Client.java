package csc435.app;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

public class Client {
    private Integer clientID;
    private String address;
    private Integer port;

    public Client(Integer clientID, String address, int port) {
        this.clientID = clientID;
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
            outputLine = "INDEX Client" + clientID.toString() + " DOC11 tiger 100 cat 10 dog 20";
            out.println(outputLine);
            // receive reply message from server (worker)
            inputLine = in.readLine();
            System.out.println("Indexing " + inputLine);
            
            // send request message to server (worker)
            outputLine = "SEARCH cat";
            out.println(outputLine);
            // receive reply message from server (worker)
            inputLine = in.readLine();
            System.out.println("Searching for cat");
            String[] tokens = inputLine.split("\\s+");
            for (int i = 0; i < tokens.length; i += 2) {
                System.out.println(tokens[i] + " " + tokens[i + 1]);
            }
            
            // send request message to server (worker)
            outputLine = "QUIT";
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
        if (args.length != 3) {
            System.err.println("USE: java Client <client ID> <IP address> <port>");
            System.exit(1);
        }

        Client client = new Client(Integer.parseInt(args[0]), args[1], Integer.parseInt(args[2]));
        client.run();
    }
}
