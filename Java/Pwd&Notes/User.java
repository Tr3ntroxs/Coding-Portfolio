import java.util.*;
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
                    System.out.println("What would you like to do?");
                    System.out.println("1. Access passwords");
                    System.out.println("2. Access notes");

                    ArrayList<String> data = new ArrayList<String>();
                    input = scanner.nextLine();
                    if(input.equals("1")){

                        System.out.println("Input one of the commands in their form: ");
                        System.out.println("_create_ _filename_");
                        System.out.println("_access_ _filename_");
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

                    }else{

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

                accounts.put(username, new Account(username, password));
            }
        }else{

            
        }
    }
}