package csc435.app;

import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;

public class Server {
    
    private String address;
    private Integer port;
    private Integer maxNumConnections;

    public Server(int port, int maxNumConnections) {
        // initialize the address so that the server will listen to all interfaces
        this.address = "0.0.0.0";
        this.port = port;
        this.maxNumConnections = maxNumConnections;
    }

    public void run() {
        try {
            ArrayList<Thread> threads = new ArrayList<Thread>();
            // create, configure and bind server socket and listen for connections
            ServerSocket serverSocket = new ServerSocket(port, maxNumConnections, InetAddress.getByName(address));

            // configure server to close after 2 clients connected
            int i = 2;

            System.out.println("Server started and waiting for connections!");

            while (i > 0) {
                // accept connection from client 
                Socket clientSocket = serverSocket.accept();
                
                // extract client information
                System.out.println("Server got connection from " + clientSocket.getInetAddress().getHostAddress());
                
                // create worker thread for new client connection
                Thread td = new Thread(new Worker(clientSocket));
                td.start();
                threads.add(td);
                i--;
            }

            serverSocket.close();

            for (Thread td : threads) {
                td.join();
            }
        } catch (UnknownHostException e) {
            System.err.println("Could not compute IP address!");
            e.printStackTrace();
        } catch (IOException e) {
            System.err.println("Socket error!");
            e.printStackTrace();
        } catch (InterruptedException e) {
            System.err.println("Thread join error!");
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        if (args.length != 1) {
            System.err.println("USE: java Server <port>");
            System.exit(1);
        }

        Server server = new Server(Integer.parseInt(args[0]), 4);
        server.run();
    }
}
