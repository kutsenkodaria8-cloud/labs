import pandas as pd
import streamlit as st
import plotly.express as px

df = pd.read_csv("vhi_data.csv")
region_names = {
1: "Вінницька", 2: "Волинська", 3: "Дніпропетровська", 4: "Донецька", 5: "Житомирська", 6: "Закарпатська",7: "Запорізька",
8: "Івано-Франківська", 9: "Київська", 10: "Кіровоградська", 11: "Луганська", 12: "Львівська", 13: "Миколаївська",
14: "Одеська", 15: "Полтавська", 16: "Рівненська", 17: "Сумська", 18: "Тернопільська", 19: "Харківська",
20: "Херсонська", 21: "Хмельницька", 22: "Черкаська", 23: "Чернівецька", 24: "Чернігівська", 25: "Республіка Крим"
}
col1, col2 = st.columns([1,2])
with (col1):
    if st.button("Скинути фільтр"):
        st.session_state["index_type"] = "VCI"
        st.session_state["region"] = list(region_names.values())[0]
        st.session_state["week"] = (1, 52)
        st.session_state["year"] = (int(df["Year"].min()), int(df["Year"].max()))
        st.session_state["sort_a"] = False
        st.session_state["sort_b"] = False
    index_type=st.selectbox(
        "Виберіть часовий ряд",
        ["VCI", "TCI", "VHI"],
    key="index_type")
    region=st.selectbox(
        "Виберіть область",
        list(region_names.values()),key="region")
    week=st.slider(
        "Тижні",
        value=(1, 52),
        min_value=1,
        max_value=52,key="week")
    year=st.slider(
        "Роки",
        value=(df["Year"].min(), df["Year"].max()),
        max_value=df["Year"].max(),
        min_value=df["Year"].min(),key="year")
    sort_a = st.checkbox("Сортування за зростанням",key="sort_a")
    sort_b = st.checkbox("Сортування за спаданням", key="sort_b")
    region_ids = {v: k for k, v in region_names.items()}
    region_id = region_ids[region]
    filtered_df = df[
        (df["ID"] == region_id) & (df["Year"] >= year[0]) & (df["Year"] <= year[1]) & (df["Week"] >= week[0]) & (
                    df["Week"] <= week[1])]

if sort_a and sort_b:
    st.warning("Помилка, оберіть одне сортування")
if sort_a:
    filtered_df = filtered_df.sort_values(by=index_type, ascending=True)
if sort_b:
    filtered_df = filtered_df.sort_values(by=index_type, ascending=False)

with (col2):
    tab1,tab2,tab3 = st.tabs(["Таблиця","Графік","Порівняння"])
    with tab1:
        st.dataframe(filtered_df)
    with tab2:
        st.line_chart(filtered_df[["Week", index_type]].set_index("Week"))
    with tab3:
        summary_df = df[
            (df["Year"] >= year[0]) & (df["Year"] <= year[1]) & (df["Week"] >= week[0]) & (df["Week"] <= week[1])]
        summary_df=summary_df.groupby("ID")[index_type].mean().reset_index()
        summary_df["Region"]=summary_df["ID"].map(region_names)
        summary_df["Color"] = summary_df["Region"].apply(lambda x: "Обрана" if x == region else "Інші")
        fig = px.bar(summary_df, x="Region", y=index_type, color="Color", color_discrete_map={"Обрана": "red", "Інші": "steelblue"})
        st.plotly_chart(fig)









