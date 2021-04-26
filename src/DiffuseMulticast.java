package src;
import java.io.*;
import java.net.*;
import java.util.*;

import src.Diffuseur;

public class DiffuseMulticast implements Runnable{
    private LinkedList<String> diffuseMsg;
    private int portMulticast;
    private String adresseMulticast;

    //Obselète, le lien avec le diffuseur est inutile
    //private Diffuseur parent;
    
    //Position du message actuellement diffusé dans la liste
    private int indice = 0;
    //Nombre total de messages diffusés jusqu'à présent
    private int msgEnvoye = 0;

    public DiffuseMulticast(LinkedList<String> diffuseMsg, int port, String adresse){
        this.diffuseMsg = diffuseMsg;
        this.portMulticast = port;
        this.adresseMulticast = adresse;
    }

    /*public void setDiffuseur(Diffuseur d){
        this.parent = d;
    }*/

    public LinkedList<String> getListMsg(){
        return this.diffuseMsg;
    }

    /* 
     * Ajoute un message dans la liste s'il est de la bonne taille (tous paramètres inclus)
     * 9 = format du nombre (ex: 0002) + \r\n + 3 espaces
     */
    public synchronized void ajoutMsg(String s){
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

    //Incrémente l'indice de diffusion 
    private synchronized void incrementeIndice(){
        this.indice = (this.indice + 1) % diffuseMsg.size();
    }

    //Assemble un message pour la diffusion
    private String assembleDiff(){
        return Diffuseur.DIFF + " " + Diffuseur.formatageEntier(msgEnvoye) + " " + this.diffuseMsg.get(indice);
    }

    public void run(){
        try{
            DatagramSocket dso = new DatagramSocket();
            byte[]data;
            InetSocketAddress ia = new InetSocketAddress(adresseMulticast, portMulticast);
            
            //Diffusion des messages
            while(true){
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
