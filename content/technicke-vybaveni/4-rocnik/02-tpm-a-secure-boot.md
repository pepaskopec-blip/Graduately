---
id: hardware-4-tpm-a-secure-boot
puvodni: hw4.2
nazev: TPM a Secure Boot
popis: Železo, které hlídá start systému.
nazev_cviceni: Kvíz: TPM a Secure Boot
nadpis_kvizu: Kvíz k tématu TPM a Secure Boot
---

# Výklad

## Klíče mimo disk
*TPM*

- Čip drží klíče použité při startu a šifrování disku.
- Bez něj některé systémy šifrování disku odmítnou.
- Vymazání TPM bez zálohy klíčů znamená ztrátu přístupu k disku.
- Na virtuálu bývá TPM také virtuální a patří k tomu stroji.

> Tip: TPM není heslo, které uživatel píše při každém startu.

## Řetěz důvěry
*Start*

- Firmware ověří zavaděč.
- Zavaděč ověří jádro.
- Cizí systém z flash disku bez podpisu nenaběhne.
- V laboratoři se výjimka zapíše, nenechá vypnutá navždy.

> Tip: Secure Boot a TPM řeší start, ne to, co uživatel spustí odpoledne.

## Kensington není šifra
*Zámek*

- Zámek skříně zpomalí toho, kdo chce odnést RAM.
- Heslo BIOSu zpomalí toho, kdo chce přeházet boot.
- Šifra disku zastaví toho, kdo disk odnese.
- Všechny tři vrstvy řeší jiného zloděje.

> Tip: lankový zámek zdrží odnesení, data chrání šifrování disku.


# Kvíz

## Co TPM drží?
- [x] Klíče pro start a šifrování
- Zálohu všech dokumentů školy
- Obraz monitoru
- Seznam VLAN
> Není to úložiště souborů.

## Co se stane po vymazání TPM bez zálohy klíčů?
- [x] Šifrovaný disk nemusí jít otevřít
- Zrychlí se start
- Smaže se jen spořič
- Nic, TPM klíče nepoužívá
> Klíč v čipu bez kopie je pryč.

## Co Secure Boot nezastaví?
- [x] Program, který uživatel spustí v už naběhlém systému
- Nepodepsaný zavaděč
- Cizí systém z flashe, pokud podpis chybí
- Změnu řetězu startu
> Hlídá start, ne každé pozdější kliknutí.

## Co ochrání data na odcizeném disku?
- [x] Šifrování disku
- Jen lankový zámek skříně
- Heslo spořiče
- Inventární štítek
> Zámek zdrží odnesení celé skříně, ne čtení disku na jiném stroji.
