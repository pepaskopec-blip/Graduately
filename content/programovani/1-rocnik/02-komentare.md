---
id: cs-1-komentare
nazev: Komentáře
popis: Poznámky, které se nespouštějí.
---

# Výklad

## Jednořádkový
*//*

- Vše za // na stejném řádku počítač přeskočí.
- Komentář vysvětluje, proč tam řádek je, ne co je vidět.
- Kód bez komentáře pořád běží stejně.

> Tip: Komentář není výpis. Na konzoli se neobjeví.

## Více řádků
*/* */*

- Blok začíná /* a končí */.
- Hodí se, když chcete dočasně vypnout několik řádků.
- Zapomenuté */ rozbije zbytek souboru.

> Tip: Nevnořujte blokové komentáře do sebe.

# Kód

```
// Tohle se nespustí.
Console.WriteLine("Tento řádek ano");
/*
Console.WriteLine("A tento ne");
*/
```
