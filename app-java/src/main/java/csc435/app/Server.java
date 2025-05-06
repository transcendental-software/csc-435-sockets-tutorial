package csc435.app;

import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Scanner;

public class Server implements Runnable {
    
    private String address;
    private Integer port;
    private Integer maxNumConnections;
    private boolean terminate;

    public Server(int port, int maxNumConnections) {
        // initialize the address so that the server will listen to all interfaces
        this.address = "0.0.0.0";
        this.port = port;
        this.maxNumConnections = maxNumConnections;
    }

    public void setTerminate() {
        this.terminate = true;
    }

    @Override
    public void run() {
        terminate = false;

        try {
            ArrayList<Thread> threads = new ArrayList<Thread>();
            // create, configure and bind server socket and listen for connections
            ServerSocket serverSocket = new ServerSocket(port, maxNumConnections, InetAddress.getByName(address));

            System.out.println("Server started and waiting for connections!");
            System.out.print("> ");

            while (true) {
                // accept connection from client 
                Socket clientSocket = serverSocket.accept();

                if (terminate) {
                    break;
                }
                
                // extract client information
                System.out.println("");
                System.out.print("Server got connection from " + clientSocket.getInetAddress().getHostAddress());
                
                // create worker thread for new client connection
                Thread td = new Thread(new Worker(clientSocket));
                td.start();
                threads.add(td);
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
        Thread serverThread = new Thread(server);
        serverThread.start();
        
        Scanner sc = new Scanner(System.in);
        String command;

        while (true) {
            command = sc.nextLine();
            
            if (command.compareTo("quit") == 0) {
                server.setTerminate();
                
                try {
                    Socket socket = new Socket(InetAddress.getByName("127.0.0.1"), Integer.parseInt(args[0]));
                    socket.close();
                } catch (UnknownHostException e) {
                    System.err.println("Could not compute IP address!");
                    e.printStackTrace();
                } catch (IOException e) {
                    System.err.println("Socket error!");
                    e.printStackTrace();
                }

                try {
                    serverThread.join();
                } catch (InterruptedException e) {
                    System.err.println("Thread join error!");
                    e.printStackTrace();
                }
                System.out.println("Server terminated!");
                break;
            }
        }

        sc.close();
    }
}
