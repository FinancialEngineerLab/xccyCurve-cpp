Calendar calendar = Taiwan();
		Date settlementDate(30, Aug, 2023);
		Settings::instance().evaluationDate() = settlementDate;
		DayCounter dayCounter = Actual365Fixed();

		// SOFR swap curve //

		DayCounter termStructureDayCounter = Actual365Fixed();
		std::vector<ext::shared_ptr<RateHelper>> sofrInstruments;

		std::map<Period, ext::shared_ptr<Quote>> longOisQuotes =
		{
			{1 * Months, ext::make_shared<SimpleQuote>(0.05325)},
			{3 * Months, ext::make_shared<SimpleQuote>(0.05398)},
			{6 * Months, ext::make_shared<SimpleQuote>(0.05455)},
			{12 * Months, ext::make_shared<SimpleQuote>(0.05376)},
			{24 * Months, ext::make_shared<SimpleQuote>(0.04778)},
			{36 * Months, ext::make_shared<SimpleQuote>(0.04392)},
			{48 * Months, ext::make_shared<SimpleQuote>(0.04182)},
			{60 * Months, ext::make_shared<SimpleQuote>(0.04061)},
			{84 * Months, ext::make_shared<SimpleQuote>(0.03918)}
		};

		auto sofr = ext::make_shared<Sofr>();

		for (const auto& q : longOisQuotes) {
			auto tenor = q.first;
			auto quote = q.second;
			auto helper = ext::make_shared<OISRateHelper>(
				2, tenor, Handle<Quote>(quote), sofr);
			sofrInstruments.push_back(helper);
		}

		auto oisSofrTermStructure = ext::make_shared<PiecewiseYieldCurve<Discount, LogLinear>>(settlementDate, sofrInstruments, termStructureDayCounter);

		oisSofrTermStructure->enableExtrapolation();

		RelinkableHandle<YieldTermStructure> discountingTermStructure;
		discountingTermStructure.linkTo(oisSofrTermStructure);
		std::cout << "forward rate e.g. = " << discountingTermStructure->forwardRate(0.5, 1, Continuous) << std::endl;

		Date maturity = calendar.advance(settlementDate, 3 * Years);
		Schedule contSchedule(settlementDate, maturity,
			Period(Quarterly),
			calendar, ModifiedFollowing,
			ModifiedFollowing,
			DateGeneration::Forward, false);

		std::cout << "coupon schedule e.g" << std::endl;
		std::cout << "# of schedules! " << contSchedule.size() << std::endl;
		for (int i = 0; i < contSchedule.size(); i++)
		{
			std::cout << contSchedule.at(i).serialNumber() << ", " << contSchedule.dates().at(i) << std::endl;
		}


		/**********************/
		std::map<Period, ext::shared_ptr<Quote>> longtwdQuotes =
		{
			{12 * Months, ext::make_shared<SimpleQuote>(0.0150)},
			{24 * Months, ext::make_shared<SimpleQuote>(0.0150)},
			{36 * Months, ext::make_shared<SimpleQuote>(0.0150)},
			{48 * Months, ext::make_shared<SimpleQuote>(0.0150)},
			{60 * Months, ext::make_shared<SimpleQuote>(0.0150)},
			{84 * Months, ext::make_shared<SimpleQuote>(0.0150)}
		};

		auto twd_index = ext::make_shared<IborIndex>(Euribor3M());
		std::vector<ext::shared_ptr<RateHelper> > twdInstruments;

		for (const auto& q : longtwdQuotes)
		{
			Handle<Quote> quoteHandle(q.second);
			//auto quote = q.second;
			auto helper = ext::make_shared<SwapRateHelper>
				(quoteHandle, q.first, Taiwan(), Quarterly, ModifiedFollowing, Actual365Fixed(), twd_index, Handle<Quote>(), 0 * Days);
			twdInstruments.push_back(helper);
		}

		auto twdTermStructure = ext::make_shared<PiecewiseYieldCurve<Discount, LogLinear>>(settlementDate, twdInstruments, Actual365Fixed());
		twdTermStructure->enableExtrapolation();

		RelinkableHandle<YieldTermStructure> twd_TermStructure;
		twd_TermStructure.linkTo(twdTermStructure);

		RelinkableHandle<YieldTermStructure> sofr_TermStructure;
		sofr_TermStructure.linkTo(oisSofrTermStructure);


		xccyCurveBuilder crsbuilder(Date(30, Aug, 2023),
			Taiwan(), 2, ModifiedFollowing,
			Actual365Fixed());
		// if TWD = EUR... /
		ext::shared_ptr<IborIndex> baseCcyIdx(new QuantLib::Taibor(3 * Months, twd_TermStructure));
		ext::shared_ptr<IborIndex> quoteCcyIdx(new Sofr(sofr_TermStructure));

		//std::vector<xccyDatum> basisData;
		//basisData.emplace_back(1, Years, 0.06 * 10000.0); // 0.00785-0.015
		//basisData.emplace_back(2, Years, 0.06 * 10000.0); // 0.00775-0.015
		//basisData.emplace_back(3, Years, 0.06 * 10000.0); // 0.00775-0.015
		//basisData.emplace_back(4, Years, 0.06 * 10000.0); // 0.00725-0.015
		//basisData.emplace_back(5, Years, 0.06 * 10000.0); // 0.00660-0.015
		std::vector<xccyDatum> basisData;
		basisData.emplace_back(1, Years, (0.00785 - 0.015) * 10000.0); // 0.00785-0.015
		basisData.emplace_back(2, Years, (0.00775 - 0.015) * 10000.0); // 0.00775-0.015
		basisData.emplace_back(3, Years, (0.00775 - 0.015) * 10000.0); // 0.00775-0.015
		basisData.emplace_back(4, Years, (0.00725 - 0.015) * 10000.0); // 0.00725-0.015
		basisData.emplace_back(5, Years, (0.00660 - 0.015) * 10000.0); // 0.00660-0.015


		RelinkableHandle <YieldTermStructure> crsCurve = crsbuilder.xccyCurve(basisData, twd_TermStructure, sofr_TermStructure,
			baseCcyIdx, quoteCcyIdx);


		for (int i = 0; i < 20; i++)
		{
			if (i == 0)
			{
				std::cout << 0.0419378 << std::endl;
			}
			else
			{
				std::cout << twd_TermStructure->forwardRate(i / 4.0, (i + 1) / 4.0, Continuous, Quarterly, true) << std::endl;
			}
		}
		std::cout << std::endl;
		for (int i = 1; i <= 20; i++)
		{
			std::cout << twd_TermStructure->discount(i / 4.0, true) << std::endl;
		}
		std::cout << std::endl;
		for (int i = 1; i <= 20; i++)
		{
			std::cout << crsCurve->discount(i / 4.0, true) << std::endl;
		}
		std::cout << std::endl;
