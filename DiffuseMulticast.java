import java.io.*;
import java.net.*;
import java.util.*;

public class DiffuseMulticast implements Runnable{
    private LinkedList<String> diffuseMsg;
    private int portMulticast;
    private String adresseMulticast;

    public DiffuseMulticast(LinkedList<String> diffuseMsg, int port, String adresse){
        this.diffuseMsg = diffuseMsg;
        this.portMulticast = port;
        this.adresseMulticast = adresse;
    }

    public LinkedList<String> getListMsg(){
        return this.diffuseMsg;
    }

    public void run(){
        try{
            DatagramSocket dso = new DatagramSocket();
            byte[]data;
            int i = 0;
            InetSocketAddress ia = new InetSocketAddress(adresseMulticast, portMulticast);
            
            //TODO : rajouter un else pour wait() le thread
            while(true){
                if (i < diffuseMsg.size()){
                    data = this.diffuseMsg.get(i).getBytes();
                    DatagramPacket msg = new DatagramPacket(data, data.length, ia);
                    i++;
                    dso.send(msg);
                    Thread.sleep(500);
                }
            }
        } catch(Exception e){
            e.printStackTrace();
        }
    }
}
