---
id: cs-3-dedicnost
nazev: Dědičnost
popis: Nová třída převezme hotovou.
---

# Výklad

## base
*: Zak*

- class Student : Zak znamená, že student je žák a má něco navíc.
- Dostane Jmeno a Vek, aniž by se psaly znovu.
- base(...) zavolá konstruktor předka.

> Tip: Student umí to, co Zak, a ještě obor.

## Je
*typ*

- Student lze uložit do proměnné Zak. Pořád je to student.
- Opačně žák bez oboru studentem není.
- Na specialitu sáhnete, až když máte původní typ.

> Tip: Vypište i obor. Patří studentovi, ne obecnému žákovi.

# Kód

```
Student s = new Student("Eva", 18, "informatika");
Console.WriteLine($"{s.Jmeno}, {s.Obor}");

class Zak
{
    public string Jmeno { get; set; }
    public int Vek { get; set; }
    public Zak(string jmeno, int vek)
    {
        Jmeno = jmeno;
        Vek = vek;
    }
}

class Student : Zak
{
    public string Obor { get; set; }
    public Student(string jmeno, int vek, string obor) : base(jmeno, vek)
    {
        Obor = obor;
    }
}
```
