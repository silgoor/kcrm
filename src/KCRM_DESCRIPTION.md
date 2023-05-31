НА БУДУЩЕЕ
Копирование заявки (создание новой по шаблону старой)


Поля таблицы «requests» (заявки)

id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
agreementid INTEGER - договор, к которому привязана заявка.
officeid INTEGER - офис, в котором заявку должны обработать
status INTEGER - состояние заявки
clientid INTEGER - клиент
performerid INTEGER - исполнитель заявки
performerbillid INTEGER - счёт мастеру, в который входит эта заявка.
rating INTEGER - оценка заявки
percent INTEGER - процент офиса, в котором мастер числился на момент создания счета
bonususer INTEGER - пользователь, получающий с заявки бонус
flags BIGINT - битовые флаги заказа. Названия флагов в таблице requestflags
devicename TEXT - модель устройства
defect TEXT - неисправность
done TEXT - выполненые работы
notes TEXT - примечания
clientcost REAL - стоимость для клиента
cost REAL - стоимость, по которой рассчитывается процент для выставления счета мастеру
collection UNSIGNED INTEGER
performdate TEXT - дата выполнения
cuserid INTEGER - создатель записи
ctime TEXT - время создания записи
mtime TEXT - время последней модификации записи

requestdevicetypes - устарела, заменена на requestflags.
id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
rid INTEGER
dtid INTEGER


requestflags
id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
bit INTEGER - номер бита (от 1 до 63)
shortname TEXT - имя д
name TEXT
rfgroupid INTEGER - индекс в таблице requestflaggroups


requestflaggroups
id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
type INTEGER - 0-галка (множественное выделение), 1-радио (единичное выделение)
name TEXT


requestperformers - БОЛЬШЕ НЕ ИСПОЛЬЗУЕТСЯ.
id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
rid INTEGER
pid INTEGER


performerbills - счета исполнителям НЕ ИСПОЛЬЗУЕТСЯ
id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
performerid INTEGER - мастер, которому выставлен счёт
amount REAL - сумма
collected UNSIGNED INTEGER - инкассирован
collect_userid INTEGER - пользователь, поставивший инкассацию.
cuserid INTEGER - создатель записи
ctime TEXT - время создания записи


Поля таблицы «clients»
id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
sex INTEGER - пол (1 - Мужской, 2 - Женский)
surname TEXT - фамилия клиента
name TEXT - имя клиента
phone1 TEXT - телефон 1
phone2 TEXT - телефон 2
email TEXT
address TEXT - адрес объекта
passport TEXT - паспортные данные
paddress TEXT - прописка
inn TEXT
kpp TEXT
okpo TEXT
bank TEXT
rschet TEXT - расчетный счет
kschet TEXT - корреспонденский счет
bik TEXT
ogrn TEXT
okved TEXT - ОКВЭД
store_address TEXT - адрес склада
store_phone TEXT - телефон склада
gendirector TEXT
manager1 TEXT
manager1_phone TEXT
manager1_email TEXT
manager2 TEXT
manager2_phone TEXT
manager2_email TEXT
site TEXT
paytype TEXT
delivery TEXT
brands TEXT
worktime TEXT
cuserid INTEGER - создатель записи
ctime TEXT - время создания записи
mtime TEXT - время последней модификации записи


Поля таблицы "states» (названия состояний заказа)

id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ 1-в работе, 10-выполнено, 20-отмена
name TEXT - название состояния


Поля таблицы «devicetypes» (названия типов оборудования)

id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
enabled INTEGER
name TEXT - название типа оборудования


Поля таблицы «devices» (оборудование) НЕ ИСПОЛЬЗУЕТСЯ

id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
devicetypeid INTEGER - тип
brand TEXT - марка
model TEXT - модель
notes TEXT - примечания


Поля таблицы «clientdevices» (оборудование клиентов) НЕ ИСПОЛЬЗУЕТСЯ

id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
clientid INTEGER
deviceid INTEGER - оборудование
serial TEXT - серийный номер
ctime TEXT


Поля таблицы "pays"
id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
requestid INTEGER - номер заказа
amount REAL - сумма
type INTEGER DEFAULT 1 - тип платежа (1 - наличные, 2 - банковская карта)
userid INTEGER - ключ пользователя в таблице users, принявшего платеж
ctime TEXT - время последней модификации записи


Поля таблицы «offices»
id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
enabled INTEGER DEFAULT 1 - удаление записи будет состоять в установке этого флага в FALSE.
performerbillpercent INTEGER - процент, на который выставляется счёт мастеру.
name TEXT
email TEXT - электронка
url TEXT - сайт
phone1 TEXT
phone2 TEXT
address TEXT
managername TEXT - Ф.И.О. менеджера
notes TEXT - примечания
ctime TEXT - время последней модификации записи


Поля таблицы "users" (пользователи программы)

id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
enabled INTEGER DEFAULT 1 - удаление записи будет состоять в установке этого флага в FALSE.
login TEXT
password TEXT
rights INTEGER - права пользователя
sex INTEGER - пол: 0 - мужчина, 1 - женщина
name TEXT
birthday TEXT
phone1 TEXT
phone2 TEXT
email TEXT
address TEXT - фактическое место жительства
pseries TEXT - серия паспорта
pnumber TEXT - номер паспорта
pdepartment TEXT - кем выдан паспорт
paddress TEXT - место регистрации
notes TEXT - примечания
rating_avg REAL - средняя оценка мастера. Вычисляется за 10 последних оцененных заявок
ctime TEXT - время создания записи
websessionuid TEXT - Уникальный идентификатор сессии в веб-админке



Поля таблицы «performeroffices» (привязка мастеров к офисам)

id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
performerid INTEGER
officeid INTEGER


Поля таблицы "options" (опции)

id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
name TEXT - название параметра
localname TEXT - название, отображаемое пользователю
description TEXT - описание
value TEXT - значение параметра
timestamp TEXT - время последнего изменения


Поля таблицы «events» (события)

id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
actionid INTEGER - действие пользователя
rowid INTEGER - строка таблицы, к которой применено действие (необязательный параметр)
userid INTEGER - ключ пользователя
ctime TEXT - время события


Поля таблицы «actions» (действия)

id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
name TEXT -  название действия


Поля таблицы «agreements» (договора)

id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ, номер договора
officeid INTEGER - офис, к которому привязан договор
type INTEGER -  вид договора
trest INTEGER - трест газового хозяйства
startdate TEXT - дата заключения
timelimit INTEGER - срок договора, месяцев (0-бессрочный)
status INTEGER - 1=обслуживается 2=расторгнут
cost DOUBLE - стоимость договора
collected UNSIGNED INTEGER - инкассированная сумма (закодирована)
client INTEGER - клиент
json TEXT - дополнительные данные в формате json
devicetype INTEGER - тип устройства - НЕ ИСПОЛЬЗУЕТСЯ
alertdate TEXT - будильник
cuser INTEGER - создатель записи
ctime TEXT - дата создания записи


Поля таблицы «agreementdevicetypes»
id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
aid INTEGER
dtid INTEGER


Поля таблицы «agreementtypes» (типы договора)

id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
enabled INTEGER DEFAULT 1 - удаление записи будет состоять в установке этого флага в FALSE.
name TEXT


Поля таблицы «trests» (тресты газового хозяйства)

id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
enabled INTEGER DEFAULT 1 - удаление записи будет состоять в установке этого флага в FALSE.
name TEXT


Поля таблицы «messages» (текстовые сообщения)

id INTEGER PRIMARY KEY AUTOINCREMENT - первичный ключ
chat_type INTEGER - тип чата (привязка сообщения к договору, заявке, или просто чату). 
chat_id INTEGER - идентификатор в таблице договоров, заявок, или чатов.
message TEXT
cuser INTEGER - создатель записи
ctime TEXT - дата создания записи
