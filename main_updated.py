from license_plate import RoLicensePlate

license_plate = RoLicensePlate("CD 123456")
print(license_plate)

#validare county
print(f"validare judet: {license_plate.validate_county()}")

    
#mai adauga validari - combinatii de litere care incep cu I sau O / III sau OOO 
print(f"validare litere: {license_plate.validate_letters()}")
print(f"validare numere: {license_plate.validate_number()}")

if license_plate.validate_county() & license_plate.validate_letters(): 
    if license_plate.validate_number():   #daca s-a trecut cu bine de verificarile din functiile validate_county() si validate_Letters()
        print("Nr.inmatriculare valid")                                      #atunci numarul de inmatriculare este valid
else: print("Nr.inmatriculare invalid")
    #de modificat cazul cu numere rosii sa nu fie invalide inainte de a verifica dfunctia cu ele

#numere rosii
if license_plate.red_license_plate():
    print("Numar rosu valid")
else:
    print("Numar rosu invalid")
    

#numere diplomatice
if license_plate.diplomatic_license_plate():
    print("Numar diplomatic valid")
else:
    print("Numar diplomatic invalid")
    
    
#numere de inmatriculare rezervate pt institutii de stat
if license_plate.government_licence_plate():
    print("Masina a unei institutii de stat")
    
    
    
    
#numerele din Bucuresti au 3 cifre
#pt mai, deschide direct bariera