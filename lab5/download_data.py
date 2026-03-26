import urllib.request
import pandas as pd
import os
from datetime import datetime
from io import StringIO

if not os.path.exists('vhi_data'):
    os.makedirs('vhi_data')

def download():
    for pid in range(1, 28):
        if any(f.startswith(f'vhi_id_{pid}_') for f in os.listdir('vhi_data')):
            continue
        url = f'https://www.star.nesdis.noaa.gov/smcd/emb/vci/VH/get_TS_admin.php?country=UKR&provinceID={pid}&year1=1981&year2=2024&type=Mean'
        now = datetime.now().strftime('%Y%m%d%H%M%S')
        urllib.request.urlretrieve(url, f'vhi_data/vhi_id_{pid}_{now}.csv')
        print(f'Завантажено область {pid}')

ID_MAP = {1: 22, 2: 24, 3: 23, 4: 25, 5: 3, 6: 4, 7: 8, 8: 19, 9: 20, 10: 21,
          11: 9, 12: 13, 13: 14, 14: 15, 15: 16, 16: 17, 17: 18, 18: 6, 19: 1,
          20: 2, 21: 7, 22: 5, 23: 10, 24: 11, 25: 12}

def clean_data(path):
    frames = []
    for f in os.listdir(path):
        if not f.endswith('.csv'):
            continue
        with open(os.path.join(path, f), encoding='utf-8', errors='ignore') as file:
            lines = file.readlines()
        lines = [l.replace('<tt><pre>', '').replace('<br>', '').strip().rstrip(',')
                 for l in lines[2:] if l.strip() and not l.strip().startswith('<')]
        tmp = pd.read_csv(StringIO('\n'.join(lines)),
                          names=['Year', 'Week', 'SMN', 'SMT', 'VCI', 'TCI', 'VHI'])
        tmp['ID'] = ID_MAP.get(int(f.split('_')[2]))
        tmp = tmp.dropna()
        tmp = tmp[tmp['VHI'] != -1]
        tmp['Year'] = pd.to_numeric(tmp['Year'], errors='coerce').astype(int)
        frames.append(tmp[tmp['Year'] >= 1981])
    return pd.concat(frames, ignore_index=True)

download()
df = clean_data('vhi_data')
df.to_csv('vhi_data.csv', index=False)
print('Збережено vhi_data.csv')