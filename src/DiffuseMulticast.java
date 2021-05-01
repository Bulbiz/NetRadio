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

    //Temps d'intervalle entre chaque diffusion de message
    private int intervalleMsg = 2000;
    
    //Position du message actuellement diffusé dans la liste
    private int indice = 0;
    //Nombre total de messages diffusés jusqu'à présent
    private int msgEnvoye = 0;
    //Nombre total de messages transmis via MESS
    private int msgUtilisateurs = 0;

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

    /* 
     * Ajoute un message dans la liste de diffusion
     * Sans vérification car le message doit d'abord passer par assembleMsgDiff()
     */
    public synchronized void ajoutMsg(String s){
        if(this.diffuseMsg.size() < 10000){
            this.diffuseMsg.add(s);
        } else {
            System.out.println("Erreur lors de l'ajout du message dans la liste de diffusion");
        }
    }

    public int getEnvoye(){
        return this.msgEnvoye;
    }

    public int getIndice(){
        return this.indice;
    }

    public int getMsgUtilisateurs(){
        return this.msgUtilisateurs;
    }

    //Incrémente l'indice de diffusion 
    private synchronized void incrementeIndice(){
        this.indice = (this.indice + 1) % diffuseMsg.size();
    }

    //Assemble un message pour la diffusion
    private String assembleDiff(){
        String msg = this.diffuseMsg.get(indice);

        if(!msg.substring(0, 8).equals(parent.getIdentifiantDiff())){
            this.msgUtilisateurs++;
        }

        return Diffuseur.DIFF + " " + Diffuseur.formatageEntier(msgEnvoye) + " " + msg;
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
                Thread.sleep(intervalleMsg);
            }
        } catch(Exception e){
            e.printStackTrace();
        }
    }
}
