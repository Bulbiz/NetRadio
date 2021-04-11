import java.net.*;
import java.util.*;
import java.io.*;
import java.lang.*;

public class EcouteUtilisateur implements Runnable{
    private Socket socket;
    private DiffuseMulticast liveStream;

    public EcouteUtilisateur(Socket s, DiffuseMulticast live){
        this.socket = s;
        this.liveStream = live;
    }

    public void run(){
        try{
            while(true){
                BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                PrintWriter pw = new PrintWriter(new OutputStreamWriter(socket.getOutputStream()));

                String msg = br.readLine();
                String [] traitement = msg.split(" ");

                if (traitement[0].equals(Diffuseur.MESS) 
                    && traitement[1].length() <= 8 
                    && traitement[2].length() <= 140) {
                    this.liveStream.getListMsg().add(traitement[2]);

                    pw.print(Diffuseur.ACKM + "\n");
                    pw.flush();

                    this.socket.close();
                    break;
                } else if (traitement[0].equals(Diffuseur.LAST)) {
                    try{
                        int nbMsg = Integer.valueOf(traitement[1]);
                        int posMsg = this.liveStream.getIndice();
                        for(int i = 0; i < nbMsg; i++){
                            if (posMsg == 0){
                                posMsg = this.liveStream.getListMsg().size() - 1;
                            }
                            String [] ancienMsg = this.liveStream.getListMsg().get(posMsg).split(" ");
                            pw.print(Diffuseur.OLDM + ancienMsg[1] + ancienMsg[2] + ancienMsg[3] + "\n");
                            pw.flush();
                            posMsg--;
                        }
                        pw.print(Diffuseur.ENDM + "\n");
                        pw.flush();
                    } catch(NumberFormatException e) {
                        pw.print("[Erreur] : Format de nombre invalide\n");
                        pw.flush();
                    }
                } else {
                    pw.print("[Erreur] : Message au mauvais format\n");
                    pw.flush();
                }
            }
        } catch(Exception e) {
            System.out.println(e);
            e.printStackTrace();
        }
    }
}
