import java.util.*;
import java.io.*;
import Account.java;

public class User{

    private static HashMap<String, Account> accounts = 
        new HashMap<String, Account>();

    public static void main(String[] args) {

        System.out.println("Welcome to my notes & password manager!");
        System.out.println("Would you like to 1. login 2. create new");
        Scanner scanner = new Scanner(System.in);
        String input = scanner.nextLine();
        
        if (input.equals("1")){

                System.out.print("Enter username: ");
                String username = scanner.nextLine();
                System.out.print("\nEnter password: ");
                String password = scanner.nextLine();

                if(accounts.get(username) != null){
                    Account acc = accounts.get(username);

                    while(true){
                        System.out.println("What would you like to do?");
                        System.out.println("1. Access passwords");
                        System.out.println("2. Access notes");

                        input = scanner.nextLine();

                        if(input.equals("1")){

                            System.out.println("Here's the following passwords: ");
                            for(String pwd : acc.get_pwds()){
                                System.out.println(pwd);
                            }

                            System.out.println("\nInput one of the commands in their form: ");
                            System.out.println("_create_ _filename_");
                            System.out.println("_open_ _filename_");
                            System.out.println("_edit_ _filename_");
                            System.out.println("_delete_ _filename_");
                            input = scanner.nextLine();

                            if(input.equals("")){

                            }else if(input.equals("")){
                                
                            }else if(input.equals("")){
                                
                            }else if(input.equals("")){
                                
                            }else{

                            }

                            data = acc.get_notes();

                        }else if(input.equals("2")){

                            System.out.println("Here's the following notes: ");
                            for(String note : acc.get_notes()){
                                System.out.println(note);
                            }

                            System.out.println("\nInput one of the commands in their form: ");
                            System.out.println("_create_ _filename_");
                            System.out.println("_open_ _filename_");
                            System.out.println("_edit_ _filename_");
                            System.out.println("_delete_ _filename_");
                            System.out.println("_exit_");
                            input = scanner.nextLine();

                            if(input.substring(0,6) == "create"){
                                try {
                                    File new_note = new File(input.substring(7));
                                    if (new_note.createNewFile()) {
                                        System.out.println("File created: " + new_note.getName());
                                        acc.get_notes().add(input.substring(7));
                                    } else {
                                        System.out.println("File already exists");
                                    }
                                } catch (IOException e) {
                                    System.out.println("An error occurred");
                                    e.printStackTrace();
                                }

                            }else if(input.substring(0,4) == "open"){
                                try {
                                    FileInputStream f = new FileInputStream(input.substring(5));
                                    int r = 0;
                                    System.out.println("Contents of file: " + input.substring(5));
                                    while(r = f.read() != -1){
                                        System.out.print((char)r);
                                    }
                                } catch (IOException e) {
                                    System.out.println("An error occurred");
                                    e.printStackTrace();
                                }

                            }else if(input.substring(0,4) == "edit"){ // Open editor for now, implement later if you want better security
                                System.out.println();

                            }else if(input.substring(0,6) == "delete"){
                                try {
                                    File new_note = File(input.substring(7));
                                    if (new_note.delete()) {
                                        System.out.println("File deleted successfully");
                                        acc.get_notes().remove(input.substring(7));
                                    } else {
                                        System.out.println("Failed to delete the file");
                                    }
                                } catch (IOException e) {
                                    System.out.println("An error occurred");
                                    e.printStackTrace();
                                }
                            }else{

                            }

                        }else{

                        }
                    }
                }else{
                    System.out.println("Username or password was not found. Try again: ");
                }

        }else if(input.equals("2")){
            System.out.print("Enter new username: ");
            String username = scanner.nextLine();
            System.out.print("\nEnter new password: ");
            String password = scanner.nextLine();
            System.out.print("\nEnter password again: ");
            String verify = scanner.nextLine();
            if(verify.equals(password)){
                accounts.put(username, new Account(username, encrypt(password)));
            }
        }else{

            
        }
    }
}