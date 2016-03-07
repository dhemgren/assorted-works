// X1 Progp15 Pebble Solitaire Dan Hemgren

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.Scanner;


public class Pebble {
	public static byte EMPTY = 45;
	public static byte PEBBLE = 111;
	public static byte MAX_COL = 12;
	public static int count;

	public static void main(String[] args) throws FileNotFoundException {
		//System.setIn(new FileInputStream("game2.in"));	-- för enkel debug i ex. Eclipse
		
		Scanner sc = new Scanner(System.in);
		int games = Integer.parseInt(sc.nextLine());
	
		for(int i = 0; i < games; i++)
			checkGame(sc.nextLine());
	}
	
	public static void checkGame(String game){
		byte[] field = game.getBytes();
		//printField(field);
		count = amountPebbles(field);
		recCheck(field);
		System.out.println(count);
	}
	
	public static void recCheck(byte[] f){
		byte[] field = f.clone(); 	// arrays ej immutable -> måste klona
		
		for(int i = 0; i < MAX_COL - 1; i++){		
			if(field[i] == PEBBLE && field[i+1] == PEBBLE){
				if(i+2 < MAX_COL && field[i+2] == EMPTY){
					field[i+2] = PEBBLE;
					field[i] = EMPTY;
					field[i+1] = EMPTY;
					recCheck(field);
					field = f;  		// återställ spelplan
				}
			
				if(i-1 >= 0 && field[i-1] == EMPTY){
					field[i-1] = PEBBLE;
					field[i] = EMPTY;
					field[i+1] = EMPTY;
					recCheck(field);
					field = f;			// återställ spelplan
				}
			}
		}
			
		int amt = amountPebbles(field);

		if(amt < count)
			count = amt;
	}

	/**
	*	Räknar kulor i angivna spelplanen
	*/
	public static int amountPebbles(byte[] field){
		int amount = 0;
		for(byte b : field){
			if(b == PEBBLE)
				amount++;
		}
		return amount;
	}
	
	/**
	*	Skriver ut spelplan till stdout
	*/
	public static void printField(byte[] field){
		for(byte b : field)
			switch(b){
				case 111: System.out.print("o");
				break;
				case 45: System.out.print("-");
				break;
			}
		System.out.println("");
	}
}
