'''
module containing classes and functions to work with romanian licese plates
'''

class RoLicensePlate:
    '''
    TODO: Validation of License plate strings 
      #county cod valid, numbers si letters sa fie valide - adica nu mai mult de trei litere/2 numere, - TEMA
    '''
    
    def __init__(self, license_str: str)->None:
        '''
        initialisies a RoLicensePlate instance form a licence number given a string
        License string may or may not contain spaces
        '''
        self.county = ""
        self.numbers = ""
        self.letters = ""
        
        #1) normalize inputs by removing spaces
        #2) iterate character by character over the string 
        #3) first letter will be county code
        #4) after county code, you have the numbers
        #5) after the numbers, we have the letters
        
        license_str = license_str.replace(" ","")
        #print(license_str)
        found_county = False
       
        if (license_str[0].isalpha() and license_str.isupper()):  #len(license_str) != 9
            for i in license_str:
                if i.isalpha():
                    if not found_county:
                        self.county += i
                    else:
                        self.letters += i
            
                elif i.isdigit():
                    found_county = True
                    self.numbers += i
        else:
            print("Numar invalid")
            exit()
                
                
        self.numbers = int(self.numbers)
        #print(self.county)
        #print("dupa corectie")
        self.correction_county()
        #print(self.county)
                
    def __repr__(self):
        return f"<RoLicensePlate county={self.county}, numbers ={self.numbers}, letters = {self.letters}"
    
    
    def validate_county(self):
        tuple_of_counties = ("B","AB","AR","AG","BC","BH","BN","BT","BV","BR","BZ","CS","CL","CJ","CT","CV","DB","DJ","GL","GR","GJ","HR","HD","IL","IS","IF","MM","MH","MS","NT","OT","PH","SM","SJ","SB","SV","TR","TM","TL","VL","VS","VN")
        license_county = self.county
        
        for county in tuple_of_counties:    #cautare in lista judetelor din Romania
            if license_county == county:
                return 1
            
        return 0
    
    
    def validate_letters(self):   #mai adauga validari - combinatii de litere care incep cu I sau O / III sau OOO 
        letters = self.letters
        count = 0
        if "Q" in letters:
            return 0
        for letter in letters:
            if letter == "I" or letter == "O":
                count += 1
            
            if count == 3:
                return 0
            
        return 1       
    
    def count_(self):
        numbers = self.numbers
        count = 0
        
        while numbers:
           numbers //= 10
           count += 1
        
        return count
           
    def validate_number(self):
       numar_cifre = self.count_()
           
       if numar_cifre == 2:
           return 1
       
       if numar_cifre == 3 and self.county == "B":
           return 1
       
       if numar_cifre == 6:
        return self.red_license_plate()

       return 0
        
        
        
    #sesizare numar de inmatriculare rezervat pentru institutiile statului
    def government_licence_plate(self):
        tuple_of_government_letters = ("GUV", "DEP", "SNT", "SRI", "POL", "MAI")
        
        for government_letters in tuple_of_government_letters:
            if government_letters == self.letters:
                return 1
        return 0
    

    #numere rosii: TM 023456
    def red_license_plate(self):
        numar_cifre = self.count_()
        
        if self.validate_county() and  numar_cifre == 6 and len(self.letters) == 0:   
            return 1
        
        return 0
        
    
    #numere diplomatice: CD/TC 123456
    def diplomatic_license_plate(self): 
        letters = self.county
        tuple_numere_diplomatice = ("CD","TC","CO")
        numar_cifre = self.count_()
        
        for numere_diplomatice in tuple_numere_diplomatice:
            if numere_diplomatice == letters:
                if numar_cifre == 6:
                    return 1
        
        return 0

    def correction_county(self):
        
        if len(self.county)>2:
            self.county = self.county[-2]+self.county[-1]
            
            
