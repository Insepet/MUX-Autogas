--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: conexion; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE conexion (
    internet boolean
);


ALTER TABLE conexion OWNER TO postgres;

--
-- Name: cortem; Type: TABLE; Schema: public; Owner: db_admin; Tablespace: 
--

CREATE TABLE cortem (
    id integer NOT NULL,
    ultima_venta integer,
    u_vol character(12),
    u_vol_2 character(12),
    u_vol_3 character(12),
    idpos character(1)
);


ALTER TABLE cortem OWNER TO db_admin;

--
-- Name: cortem_id_seq; Type: SEQUENCE; Schema: public; Owner: db_admin
--

CREATE SEQUENCE cortem_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE cortem_id_seq OWNER TO db_admin;

--
-- Name: cortem_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: db_admin
--

ALTER SEQUENCE cortem_id_seq OWNED BY cortem.id;


--
-- Name: productos; Type: TABLE; Schema: public; Owner: db_admin; Tablespace: 
--

CREATE TABLE productos (
    diesel integer,
    corriente integer,
    extra integer,
    s_diesel integer,
    id integer
);


ALTER TABLE productos OWNER TO db_admin;

--
-- Name: recibo; Type: TABLE; Schema: public; Owner: db_admin; Tablespace: 
--

CREATE TABLE recibo (
    linea1 character varying(30),
    linea2 character varying(80),
    nit character varying(15),
    tel character varying(12),
    dir character varying(30),
    footer character varying(30),
    url character varying(75),
    url_save character varying(75),
    idestacion character varying(5),
    offsetid integer
);


ALTER TABLE recibo OWNER TO db_admin;

--
-- Name: recuperacion; Type: TABLE; Schema: public; Owner: db_admin; Tablespace: 
--

CREATE TABLE recuperacion (
    idpos integer,
    tot1 character varying(12),
    tot2 character varying(12),
    tot3 character varying(12)
);


ALTER TABLE recuperacion OWNER TO db_admin;

--
-- Name: strtran; Type: TABLE; Schema: public; Owner: db_admin; Tablespace: 
--

CREATE TABLE strtran (
    idstring integer NOT NULL,
    envio text,
    respuesta text
);


ALTER TABLE strtran OWNER TO db_admin;

--
-- Name: strtran_idstring_seq; Type: SEQUENCE; Schema: public; Owner: db_admin
--

CREATE SEQUENCE strtran_idstring_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE strtran_idstring_seq OWNER TO db_admin;

--
-- Name: strtran_idstring_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: db_admin
--

ALTER SEQUENCE strtran_idstring_seq OWNED BY strtran.idstring;


--
-- Name: venta; Type: TABLE; Schema: public; Owner: db_admin; Tablespace: 
--

CREATE TABLE venta (
    autorizacion character varying(38),
    id_venta character varying(25),
    id_estacion character varying(4),
    serial character varying(16),
    km character varying(10),
    cara character(1),
    producto character(1),
    precio character varying(5),
    dinero character varying(7),
    volumen character varying(7),
    fecha character varying(20),
    enviada boolean DEFAULT false,
    id integer NOT NULL,
    placa character varying(10),
    direccion character varying(50),
    idestacion character varying(5),
    nombrecuenta character varying(255),
    telefono character varying(30)
);


ALTER TABLE venta OWNER TO db_admin;

--
-- Name: venta_id_seq; Type: SEQUENCE; Schema: public; Owner: db_admin
--

CREATE SEQUENCE venta_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE venta_id_seq OWNER TO db_admin;

--
-- Name: venta_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: db_admin
--

ALTER SEQUENCE venta_id_seq OWNED BY venta.id;


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: db_admin
--

ALTER TABLE ONLY cortem ALTER COLUMN id SET DEFAULT nextval('cortem_id_seq'::regclass);


--
-- Name: idstring; Type: DEFAULT; Schema: public; Owner: db_admin
--

ALTER TABLE ONLY strtran ALTER COLUMN idstring SET DEFAULT nextval('strtran_idstring_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: db_admin
--

ALTER TABLE ONLY venta ALTER COLUMN id SET DEFAULT nextval('venta_id_seq'::regclass);


--
-- Data for Name: conexion; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY conexion (internet) FROM stdin;
t
\.


--
-- Data for Name: cortem; Type: TABLE DATA; Schema: public; Owner: db_admin
--

COPY cortem (id, ultima_venta, u_vol, u_vol_2, u_vol_3, idpos) FROM stdin;
1	1	000000056078	000000011969	000000011904	1
2	2	000000045844	000000014947	000000011710	2
4	1	000000070944	000000000000	000000000000	1
5	3	000000071002	000000000000	000000000000	1
6	2	000009520048	000000000000	000000000000	2
7	2	000009520048	000000000000	000000000000	2
8	2	000009520048	000000000000	000000000000	2
9	2	000009520048	000000000000	000000000000	2
10	3	000079560579	000000000000	000000000000	1
\.


--
-- Name: cortem_id_seq; Type: SEQUENCE SET; Schema: public; Owner: db_admin
--

SELECT pg_catalog.setval('cortem_id_seq', 10, true);


--
-- Data for Name: productos; Type: TABLE DATA; Schema: public; Owner: db_admin
--

COPY productos (diesel, corriente, extra, s_diesel, id) FROM stdin;
1	0	0	0	1
1	0	0	0	2
\.


--
-- Data for Name: recibo; Type: TABLE DATA; Schema: public; Owner: db_admin
--

COPY recibo (linea1, linea2, nit, tel, dir, footer, url, url_save, idestacion, offsetid) FROM stdin;
PRUEBAS MEDELLIN	TERPEL MAYORISTA	900.123.456-1	555-555	MEDELLIN	GRACIAS POR SU VISITA	http://mol.autogas.com.co/ServicioGRPAliados/AT0001.svc	http://mol.autogas.com.co/ServicioGRPAliados/CV0001.svc	4	0
\.


--
-- Data for Name: recuperacion; Type: TABLE DATA; Schema: public; Owner: db_admin
--

COPY recuperacion (idpos, tot1, tot2, tot3) FROM stdin;
2	000009520048	000000000000	000000000000
1	000079560579	000000000000	000000000000
\.


--
-- Data for Name: strtran; Type: TABLE DATA; Schema: public; Owner: db_admin
--

COPY strtran (idstring, envio, respuesta) FROM stdin;
1	[object Object]	{"aT0001responseREST":{"properties":{},"cantidadAutorizada":{"properties":{},"value":0},"codigoRetorno":{"properties":{},"value":1401},"descripcionError":{"properties":{},"value":"EL KILOMETRAJE INGRESADO ES INFERIOR AL ULTIMO INGRESADO"},"direccion":{"properties":{},"value":"av Cra 20 # 80-45"},"idproducto":{"properties":{},"value":1},"nombreCuenta":{"properties":{},"value":"Pruebas"},"numeroAutorizacion":{"properties":{},"value":"00000000-0000-0000-0000-000000000000"},"placa":{"properties":{},"value":"ASD652"},"retorno":{"properties":{},"value":"N"},"telefono":{"properties":{},"value":6345170},"tipoConvenio":{"properties":{},"value":1},"tipoRetorno":{"properties":{},"value":0},"trama":{"properties":{},"value":"280000011CC9F101"},"valorConvenio":{"properties":{},"value":null}}}
2	http://mol.autogas.com.co/ServicioGRPAliados/AT0001.svc/rest/Authorize/280000011CC9F101/1/0027/08060/3/9990000/0012378	{"aT0001responseREST":{"properties":{},"cantidadAutorizada":{"properties":{},"value":16120},"codigoRetorno":{"properties":{},"value":0},"descripcionError":{"properties":{},"value":"EXITO"},"direccion":{"properties":{},"value":"av Cra 20 # 80-45"},"idproducto":{"properties":{},"value":1},"nombreCuenta":{"properties":{},"value":"Pruebas"},"numeroAutorizacion":{"properties":{},"value":"a998ff24-a183-4c34-bfc8-3f82b72e53be"},"placa":{"properties":{},"value":"ASD652"},"retorno":{"properties":{},"value":"S"},"telefono":{"properties":{},"value":6345170},"tipoConvenio":{"properties":{},"value":1},"tipoRetorno":{"properties":{},"value":1},"trama":{"properties":{},"value":"280000011CC9F101"},"valorConvenio":{"properties":{},"value":null}}}
3	http://mol.autogas.com.co/ServicioGRPAliados/AT0001.svc/rest/Authorize/280000011CC9F101/1/0027/00006/3/9990000/0010000	{"aT0001responseREST":{"properties":{},"cantidadAutorizada":{"properties":{},"value":0},"codigoRetorno":{"properties":{},"value":350},"descripcionError":{"properties":{},"value":"EL VEHICULO SE ENCUENTRA EN LA EDS CONSUMIENDO"},"direccion":{"properties":{},"value":"av Cra 20 # 80-45"},"idproducto":{"properties":{},"value":1},"nombreCuenta":{"properties":{},"value":"Pruebas"},"numeroAutorizacion":{"properties":{},"value":"00000000-0000-0000-0000-000000000000"},"placa":{"properties":{},"value":"ASD652"},"retorno":{"properties":{},"value":"N"},"telefono":{"properties":{},"value":6345170},"tipoConvenio":{"properties":{},"value":1},"tipoRetorno":{"properties":{},"value":0},"trama":{"properties":{},"value":"280000011CC9F101"},"valorConvenio":{"properties":{},"value":null}}}
4	http://mol.autogas.com.co/ServicioGRPAliados/AT0001.svc/rest/Authorize/280000011CC9F101/1/0027/00006/3/9990000/0010000	{"aT0001responseREST":{"properties":{},"cantidadAutorizada":{"properties":{},"value":0},"codigoRetorno":{"properties":{},"value":1401},"descripcionError":{"properties":{},"value":"EL KILOMETRAJE INGRESADO ES INFERIOR AL ULTIMO INGRESADO"},"direccion":{"properties":{},"value":"av Cra 20 # 80-45"},"idproducto":{"properties":{},"value":1},"nombreCuenta":{"properties":{},"value":"Pruebas"},"numeroAutorizacion":{"properties":{},"value":"00000000-0000-0000-0000-000000000000"},"placa":{"properties":{},"value":"ASD652"},"retorno":{"properties":{},"value":"N"},"telefono":{"properties":{},"value":6345170},"tipoConvenio":{"properties":{},"value":1},"tipoRetorno":{"properties":{},"value":0},"trama":{"properties":{},"value":"280000011CC9F101"},"valorConvenio":{"properties":{},"value":null}}}
5	http://mol.autogas.com.co/ServicioGRPAliados/AT0001.svc/rest/Authorize/280000011CC9F101/1/0027/00006/3/9990000/0020000	{"aT0001responseREST":{"properties":{},"cantidadAutorizada":{"properties":{},"value":12},"codigoRetorno":{"properties":{},"value":0},"descripcionError":{"properties":{},"value":"EXITO"},"direccion":{"properties":{},"value":"av Cra 20 # 80-45"},"idproducto":{"properties":{},"value":1},"nombreCuenta":{"properties":{},"value":"Pruebas"},"numeroAutorizacion":{"properties":{},"value":"a186dac5-f252-423d-948c-2ec5db37a481"},"placa":{"properties":{},"value":"ASD652"},"retorno":{"properties":{},"value":"S"},"telefono":{"properties":{},"value":6345170},"tipoConvenio":{"properties":{},"value":1},"tipoRetorno":{"properties":{},"value":1},"trama":{"properties":{},"value":"280000011CC9F101"},"valorConvenio":{"properties":{},"value":null}}}
6	http://mol.autogas.com.co/ServicioGRPAliados/AT0001.svc/rest/Authorize/050000093FFC4901/1/0027/00006/3/9990000/0020000	{"aT0001responseREST":{"properties":{},"cantidadAutorizada":{"properties":{},"value":0},"codigoRetorno":{"properties":{},"value":1401},"descripcionError":{"properties":{},"value":"EL KILOMETRAJE INGRESADO ES INFERIOR AL ULTIMO INGRESADO"},"direccion":{"properties":{},"value":"av Cra 20 # 80-45"},"idproducto":{"properties":{},"value":1},"nombreCuenta":{"properties":{},"value":"Pruebas"},"numeroAutorizacion":{"properties":{},"value":"00000000-0000-0000-0000-000000000000"},"placa":{"properties":{},"value":"AMERICANA1"},"retorno":{"properties":{},"value":"N"},"telefono":{"properties":{},"value":6345170},"tipoConvenio":{"properties":{},"value":1},"tipoRetorno":{"properties":{},"value":0},"trama":{"properties":{},"value":"050000093FFC4901"},"valorConvenio":{"properties":{},"value":null}}}
\.


--
-- Name: strtran_idstring_seq; Type: SEQUENCE SET; Schema: public; Owner: db_admin
--

SELECT pg_catalog.setval('strtran_idstring_seq', 6, true);


--
-- Data for Name: venta; Type: TABLE DATA; Schema: public; Owner: db_admin
--

COPY venta (autorizacion, id_venta, id_estacion, serial, km, cara, producto, precio, dinero, volumen, fecha, enviada, id, placa, direccion, idestacion, nombrecuenta, telefono) FROM stdin;
1	\N	\N	\N	\N	2	\N	\N	\N	0	\N	t	2	\N	\N	\N	\N	\N
1	\N	\N	\N	0	1	\N	\N	\N	0	\N	t	1	\N	\N	\N	\N	\N
00000000-0000-0000-0000-000000000000	9206386	\N	0000000000000000	0	1	1	06969	0004063	0.583	17-3-2017 9_8	t	3	\N	\N	0027	\N	\N
a998ff24-a183-4c34-bfc8-3f82b72e53be	\N	\N	280000011CC9F101	0012378	1	1	08060	\N	\N	21-9-2017 11_5	f	4	ASD652	av Cra 20 # 80-45	0027	Pruebas	6345170
a186dac5-f252-423d-948c-2ec5db37a481	\N	\N	280000011CC9F101	0020000	1	1	00006	\N	\N	21-9-2017 11_11	f	5	ASD652	av Cra 20 # 80-45	0027	Pruebas	6345170
\.


--
-- Name: venta_id_seq; Type: SEQUENCE SET; Schema: public; Owner: db_admin
--

SELECT pg_catalog.setval('venta_id_seq', 5, true);


--
-- Name: cortem_pkey; Type: CONSTRAINT; Schema: public; Owner: db_admin; Tablespace: 
--

ALTER TABLE ONLY cortem
    ADD CONSTRAINT cortem_pkey PRIMARY KEY (id);


--
-- Name: venta_pkey; Type: CONSTRAINT; Schema: public; Owner: db_admin; Tablespace: 
--

ALTER TABLE ONLY venta
    ADD CONSTRAINT venta_pkey PRIMARY KEY (id);


--
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


--
-- Name: conexion; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE conexion FROM PUBLIC;
REVOKE ALL ON TABLE conexion FROM postgres;
GRANT ALL ON TABLE conexion TO postgres;
GRANT ALL ON TABLE conexion TO db_admin;


--
-- PostgreSQL database dump complete
--

