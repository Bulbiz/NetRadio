package src;
import java.io.*;
import java.net.*;
import java.util.*;

public class DiffuseMulticast implements Runnable{
    private LinkedList<String> diffuseMsg;
    private int portMulticast;
    private String adresseMulticast;
    private Diffuseur parent;
    private int indice = 0;

    public DiffuseMulticast(LinkedList<String> diffuseMsg, int port, String adresse){
        this.diffuseMsg = diffuseMsg;
        this.portMulticast = port;
        this.adresseMulticast = adresse;
    }

    public void setDiffuseur(Diffuseur d){
        this.parent = d;
    }

    public LinkedList<String> getListMsg(){
        return this.diffuseMsg;
    }

    public void ajoutMsg(String s){
        if(s.length() <= Diffuseur.TAILLEMAXMSG){
            this.diffuseMsg.add(s);
        }
    }

    public int getIndice(){
        return this.indice;
    }

    private synchronized void incrementeIndice(){
        this.indice++;
    }

    public void run(){
        try{
            DatagramSocket dso = new DatagramSocket();
            byte[]data;
            InetSocketAddress ia = new InetSocketAddress(adresseMulticast, portMulticast);
            
            while(true){
                if (indice >= diffuseMsg.size()){
                    indice = 0;
                }
                data = this.diffuseMsg.get(indice).getBytes();
                DatagramPacket msg = new DatagramPacket(data, data.length, ia);
                incrementeIndice();
                dso.send(msg);
                Thread.sleep(500);
            }
        } catch(Exception e){
            e.printStackTrace();
        }
    }
}
