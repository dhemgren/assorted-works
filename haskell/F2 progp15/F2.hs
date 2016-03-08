module F2 where
import Data.List
import Data.Maybe

class Evol a where
    name :: a -> String             -- Ger namnet på en Evol
    distance :: a -> a -> Double    -- Mäter avstånd mellan två Evol
    evolType :: a -> String

    distanceMatrix :: [a] -> [(String, String, Double)]
    distanceMatrix evols = distanceMatrixHelper evols []

    distanceMatrixHelper :: [a] -> [(String, String, Double)] -> [(String, String, Double)]
    distanceMatrixHelper [] res = res
    distanceMatrixHelper (x:xs) res = distanceMatrixHelper xs (res++zippedList)
        where
            listA = replicate (length (x:xs)) (name x) -- Lista m. ["A",..,"A"] där A är namnet på Evol:n som jämförs
            listB = map name (x:xs)                    -- Lista m. samtliga kvarstående namn på Evol:arna
            listDistance = makeDistanceList x (x:xs) [] -- Lista m. doubles som representerar avståndet mellan A och alla i listB
            zippedList = zip3 listA listB listDistance
    
    makeDistanceList :: a -> [a] -> [Double] -> [Double]
    makeDistanceList _ [] res = reverse res
    makeDistanceList comparer comparees res = makeDistanceList comparer (tail comparees) distances
        where distances = (distance comparer (head comparees)):res

data MolSeq = MolSeq { seqName :: String, seqSequence :: String, seqType :: String }
instance Show MolSeq where
    show (MolSeq seqName seqSequence seqType) = "Name of MolSeq: " ++ seqName ++ 
                                                "\nSequence: " ++ seqSequence ++ 
                                                "\nSequence type: " ++ seqType
instance Evol MolSeq where
    name m = seqName m
    distance mA mB = seqDistance mA mB
    evolType _ = "MolSeq"

data Profile = Profile { profileName :: String, matrix :: [[(Char, Int)]], seqCount :: Int, profileType :: String }
instance Show Profile where
    show (Profile profileName matrix seqCount profileType) = "Profile: " ++ profileName ++ 
                                                             "\nMatrix: " ++ (show matrix) ++ 
                                                             "\nSequence Count: " ++ (show seqCount) ++ 
                                                             "\nProfile type: " ++ profileType
instance Evol Profile where
    name p = profileName p
    distance pA pB = profileDistance pA pB
    evolType _ = "Profile"

string2seq :: String -> String -> MolSeq
string2seq _ [] = error "Namn saknas"
string2seq [] _ = error "Sekvens saknas"
string2seq seqName seqSequence = s2sHelper seqName seqSequence [] "DNA" 

-- Går rekursivt igenom den sekvens som anges som andra argument i funktionsanropet.
-- Om ett tecken som ej är A,C,G eller T hittas så hanteras sekvensen som protein,
-- annars som DNA.
s2sHelper :: String -> String -> String -> String -> MolSeq
s2sHelper seqName seqSequence scannedseq seqType
    | null seqSequence && (seqType /= "DNA")      = MolSeq seqName (reverse scannedseq) "Protein"    -- exit conditions
    | null seqSequence && (seqType == "DNA")      = MolSeq seqName (reverse scannedseq) "DNA"     -- exit conditions
    | not((head seqSequence) `elem` nucleotides)       = s2sHelper seqName (tail seqSequence) ((head seqSequence):scannedseq) "Protein" -- icke-DNA-element funnet
    | otherwise                                   = s2sHelper seqName (tail seqSequence) ((head seqSequence):scannedseq) seqType

seqLength :: MolSeq -> Int
seqLength (MolSeq _ seqSequence _) = length seqSequence

-- Efter den första vakten vet vi att seqA == seq B, och använder sen seqA fastän båda är samma
seqDistance :: MolSeq -> MolSeq -> Double
seqDistance (MolSeq nameA seqA seqTypeA) (MolSeq nameB seqB seqTypeB)
    | seqTypeA /= seqTypeB                  = error "Protein / DNA Mismatch detected!"
    | seqTypeBoth == "DNA" && alpha > 0.74     = 3.3
    | seqTypeBoth == "Protein" && alpha > 0.94 = 3.7
    | otherwise                             = seqDistanceHelper seqTypeBoth alpha
    where alpha = seqDeviations seqA seqB 0 / fromIntegral (length seqA) -- number of deviations / length of sequence
          seqTypeBoth = seqTypeA

-- This is where the actual calculation is made
seqDistanceHelper :: String -> Double -> Double
seqDistanceHelper seqType alpha
    | seqType == "DNA"          = -(3/4) * log (1 - (4 * alpha)/3)
    | seqType == "Protein"      = -(19/20) * log (1 - (20 * alpha)/19)

-- Returns the number of deviations between seqA and seqB
seqDeviations :: String -> String -> Int -> Double
seqDeviations seqA seqB deviations
    | null seqA                 = fromIntegral deviations
    | head seqA == head seqB    = seqDeviations (tail seqA) (tail seqB) deviations
    | otherwise                 = seqDeviations (tail seqA) (tail seqB) (deviations + 1)


nucleotides = "ACGT"
aminoacids = sort "ARNDCEQGHILKMFPSTWYVX"

makeProfileMatrix :: [MolSeq] -> [[(Char, Int)]]
makeProfileMatrix [] = error "Empty sequence list" 
makeProfileMatrix sl = res
    where
        defaults =
         if seqType (head sl) == "DNA" then
          zip nucleotides (replicate (length nucleotides) 0) -- Rad (ia)         Bildar en lista [(A,0)..(T,0)], representerar DNA
         else
          zip aminoacids (replicate (length aminoacids) 0)   -- Rad (ib)        Bildar en lista [(A,0)..(X,0)], representerar Protein
        
        strings = map seqSequence sl                         -- Rad (ii)       Sekvenserna i varje MolSeq hämtas och sparas i en lista
                                                             -- Ex: strings = ["ATT", "TAA", "ATA"]
        
        transposedStrings = transpose strings                -- ["ATA", "TAT", "TAA"] 
                                                             -- Första strängen representerar nu position 1 i sekvensen

        sortedStrings = map sort transposedStrings           -- ["AAT", "ATT", "AAT"]

        groupedStrings = map group sortedStrings             -- [["AA", "T"], ["A", "TT"], ["AA", "T"]]

        tuples = map (map makeATuple) groupedStrings         -- [[(A,2), (T,1)], [(A,1), (T,2)], [(A,2), (T,1)]]
                                                             -- Antal 'A' på plats 1: 2, antal T på plats 1: 1 osv.
        
        res = map sort (map (\l -> unionBy equalFst l defaults) tuples) -- [[(A,2), (C,0), (G,0), (T,1)], [(A,1), (C,0), (G,0), (T,2)], [(A,2), (C,0), (G,0), (T,1)]]
                                                                      -- "Fyller ut" barnlistorna med icke förekommande bokstäver
                                                                      -- genom att merga med 'defaults' samt sorterar barnlistorna

        equalFst a b = (fst a) == (fst b)       -- Enkel hjälpmetod som jämför de första elementen i två tupler
        makeATuple str = (head str, length str) -- Hjälpmetod som tar en sträng "AAA" och returnerar (A,3)

-- Returnerar en profil från de givna sekvenserna med den givna strängen som namn
molseqs2profile :: String -> [MolSeq] -> Profile
molseqs2profile name molSeqs = Profile { profileName = name
                                       , matrix = makeProfileMatrix molSeqs
                                       , seqCount = length molSeqs
                                       , profileType = seqType (head molSeqs)
                                       }

-- Returnerar värdet på elementet m_c,i i profilens matris
profileFrequency :: Profile -> Int -> Char -> Double
profileFrequency profile position character = fromIntegral frequency / fromIntegral (seqCount profile)
    where
        profileMatrix = matrix profile
        childList = profileMatrix !! position       -- (Vi utgår här från att 'position' gäller en nollindexerad lista)
        tupleNumber =
            if profileType profile == "DNA" then
                elemIndex character nucleotides     -- Hitta rätt index för det givna tecknet
            else
                elemIndex character aminoacids

        frequency = snd (childList !! fromJust(tupleNumber))

-- Returnerar avståndet mellan två profiler
profileDistance :: Profile -> Profile -> Double
profileDistance profileA profileB = res
    where
        relA       = relativeFrequency (matrix profileA) (seqCount profileA)
        relB       = relativeFrequency (matrix profileB) (seqCount profileB)
        sub        = zipWith (zipWith (-))          -- En funktionsdefinition för att förenkla nästa rad.
        diffMatrix = map (map abs) (sub relA relB)  -- Detta är en matris med absolutbeloppet av (profil 1 - profil 2) på varje element.
        res        = sum (map sum diffMatrix)       -- Summerar först inom barnlistorna, och sedan inom förälderlistan.

-- Returnerar en matris med relativa frekvenser
relativeFrequency :: [[(Char,Int)]] -> Int -> [[Double]]
relativeFrequency matrix seqCount= map (map (\x -> fromIntegral x / fromIntegral (seqCount))) f
    where f = map (map snd) matrix -- Tar ut frekvenserna ur varje tupel. ('A', 2) -> 2




