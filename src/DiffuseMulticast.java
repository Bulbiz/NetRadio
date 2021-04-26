package src;
import java.io.*;
import java.net.*;
import java.util.*;

import src.Diffuseur;

public class DiffuseMulticast implements Runnable{
    private LinkedList<String> diffuseMsg;
    private int portMulticast;
    private String adresseMulticast;
    private Diffuseur parent;
    private int indice = 0;
    private int msgEnvoye = 0;

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

    //9 = format du nombre (ex: 0002) + \r\n + 3 espaces
    public void ajoutMsg(String s){
        if(s.length() <= Diffuseur.TAILLEMAXMSG + Diffuseur.TAILLEID + Diffuseur.DIFF.length() + 9){
            this.diffuseMsg.add(s);
        } else {
            System.out.println("Erreur ajout msg");
        }
    }

    public int getEnvoye(){
        return this.msgEnvoye;
    }

    public int getIndice(){
        return this.indice;
    }

    private synchronized void incrementeIndice(){
        this.indice = (this.indice + 1) % diffuseMsg.size();
    }

    private String assembleDiff(){
        return Diffuseur.DIFF + " " + Diffuseur.formatageEntier(msgEnvoye) + " " + this.diffuseMsg.get(indice);
    }

    public void run(){
        try{
            DatagramSocket dso = new DatagramSocket();
            byte[]data;
            InetSocketAddress ia = new InetSocketAddress(adresseMulticast, portMulticast);
            
            while(true){
                /*if (indice >= diffuseMsg.size()){
                    indice = 0;
                }*/
                data = assembleDiff().getBytes();
                DatagramPacket msg = new DatagramPacket(data, data.length, ia);
                dso.send(msg);
                incrementeIndice();
                msgEnvoye++;
                Thread.sleep(2000);
            }
        } catch(Exception e){
            e.printStackTrace();
        }
    }
}
