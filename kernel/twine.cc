#అన్తర్భావ "kernel/twine.h"
#అన్తర్భావ "kernel/hashcons_impl.h"
#అన్తర్భావ "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

మూస నిర్మాణం HashConsPool<TwinePool, Twine_కణుపు, IdString>;

std::సదిశ<Twine_కణుపు> StaticTwines::nodes_;

శూన్య StaticTwines::init() {
	ఒకవేళ (ready())
		తిరిగి;
	log_assert(nodes_.ఖాళీలు());
	nodes_.reserve(STATIC_TWINE_చివర);
	కొరకు (స్థిరరాశి అక్షరం *name : ID::static_పేర్లు)
		nodes_.emplace_వెనుక(std::string_view(name));
}

స్థిరరాశి Twine_కణుపు &StaticTwines::కణుపు(కొలత_t idx) { తిరిగి nodes_[idx]; }
తార్కిక StaticTwines::ready() { తిరిగి nodes_.కొలత() == STATIC_TWINE_చివర; }

int64_t twine_gc_ns;
పూర్ణాంకం twine_gc_లెక్క;

std::string IdString::handle_token() స్థిరరాశి {
	తిరిగి stringf("%s@%zu", is_ప్రజా() ? "$pub" : "$priv", untag().raw());
}

కొలత_t IdString::handle_token_ఉపసర్గ(std::string_view token, తార్కిక &is_ప్రజా_2) {
	ఒకవేళ (token.substr(0, 5) == "$pub@") {
		is_ప్రజా_2 = నిజం;
		తిరిగి 5;
	}
	ఒకవేళ (token.substr(0, 6) == "$priv@") {
		is_ప్రజా_2 = అబద్ధం;
		తిరిగి 6;
	}
	తిరిగి 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_చివర);
	std::string ఫలితం = ref.is_ప్రజా() ? "\\" : "";
	ఫలితం += static_పేర్లు[idx.raw()];
	తిరిగి ఫలితం;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_చివర);
	తిరిగి static_పేర్లు[idx.raw()];
}

TwineSpec::TwineSpec(ఆకు v) : దత్తాంశం(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : దత్తాంశం(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : దత్తాంశం(std::move(v)) {}

శూన్య SmallString::store(std::string_view content)
{
	log_assert(content.కొలత() <= MAX_LEN);
	release();
	len_ = content.కొలత();
	ఒకవేళ (len_ > INLINE_CAP) {
		ptr_ = కొత్త అక్షరం[len_];
		జ్ఞాపకం_నకలు(ptr_, content.దత్తాంశం(), len_);
	} లేకపోతే {
		జ్ఞాపకం_నకలు(inl_, content.దత్తాంశం(), len_);
	}
}

శూన్య SmallString::release()
{
	ఒకవేళ (len_ > INLINE_CAP)
		తొలగించు[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(స్థిరరాశి SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	జ్ఞాపకం_నకలు(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::క్రియాకారకం=(స్థిరరాశి SmallString &other)
{
	ఒకవేళ (ఇది != &other)
		store(other.view());
	తిరిగి *ఇది;
}

SmallString &SmallString::క్రియాకారకం=(SmallString &&other) noexcept
{
	ఒకవేళ (ఇది == &other)
		తిరిగి *ఇది;
	release();
	జ్ఞాపకం_నకలు(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	తిరిగి *ఇది;
}

SmallString::~SmallString() { release(); }

Twine_కణుపు::Twine_కణుపు(Twine_కణుపు &&other) noexcept
	: పాఠ్యం_2(std::move(other.పాఠ్యం_2)), ఉపసర్గ_3(other.ఉపసర్గ_3), hash_(other.hash_)
{
	other.ఉపసర్గ_3 = DEAD;
}

Twine_కణుపు &Twine_కణుపు::క్రియాకారకం=(Twine_కణుపు &&other) noexcept
{
	ఒకవేళ (ఇది == &other)
		తిరిగి *ఇది;
	పాఠ్యం_2 = std::move(other.పాఠ్యం_2);
	ఉపసర్గ_3 = other.ఉపసర్గ_3;
	hash_ = other.hash_;
	other.ఉపసర్గ_3 = DEAD;
	తిరిగి *ఇది;
}

std::string TwineSpec::content_str() స్థిరరాశి {
	ఒకవేళ (స్వయంచాలక *ఆకు_2 = std::get_if<ఆకు>(&దత్తాంశం))
		తిరిగి ఆకు_2->s;
	స్వయంచాలక &autosfx = std::get<AutoSuffix>(దత్తాంశం);
	తిరిగి *autosfx.ఉపసర్గ_2 + autosfx.తోక;
}

స్థిర std::pair<std::string_view, తార్కిక> unescape_name(std::string_view name) {
	ఒకవేళ (!name.ఖాళీలు() && name[0] == '\\')
		తిరిగి {name.substr(1), నిజం};
	తిరిగి {name, అబద్ధం};
}

స్థిర శూన్య check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	కొరకు (అక్షరం ch : name)
		ఒకవేళ ((గుర్తులేని)ch <= (గుర్తులేని)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(తదుపరి_serial()) {}
TwinePool::TwinePool(స్థిరరాశి TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(తదుపరి_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(తదుపరి_serial()) {}

TwinePool& TwinePool::క్రియాకారకం=(స్థిరరాశి TwinePool& other) {
	ఒకవేళ (ఇది == &other)
		తిరిగి *ఇది;
	HashConsPool::క్రియాకారకం=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = తదుపరి_serial();
	తిరిగి *ఇది;
}

TwinePool& TwinePool::క్రియాకారకం=(TwinePool&& other) {
	ఒకవేళ (ఇది == &other)
		తిరిగి *ఇది;
	HashConsPool::క్రియాకారకం=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = తదుపరి_serial();
	తిరిగి *ఇది;
}

తార్కిక TwinePool::owns(IdString ref) స్థిరరాశి {
	తిరిగి ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) స్థిరరాశి {
	ఒకవేళ (ref == IdString::Null || ID::is_static(ref))
		తిరిగి ref;
	తిరిగి ref.stamped(serial_);
}

శూన్య TwinePool::check_owned(IdString ref) స్థిరరాశి {
	ఒకవేళ constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

స్థిరరాశి Twine_కణుపు& TwinePool::క్రియాకారకం[](IdString ref) స్థిరరాశి {
	check_owned(ref);
	తిరిగి HashConsPool::క్రియాకారకం[](ref);
}

కొలత_t TwinePool::gc(pool<IdString>& roots) {
	కొరకు (స్వయంచాలక &it : auto_prefixes)
		roots.చొప్పించు(it.రెండవ);
	తిరిగి HashConsPool::gc(roots);
}

స్థిరరాశి Twine_కణుపు& TwinePool::static_కణుపు(కొలత_t idx) { తిరిగి StaticTwines::కణుపు(idx); }
శూన్య TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

కొలత_t TwinePool::తదుపరి_serial() {
	స్థిర కొలత_t counter = 0;
	కొలత_t serial = ++counter;
	ఒకవేళ constexpr (IdString::MAX_SERIAL == 0)
		తిరిగి serial;
	లేకపోతే
		తిరిగి serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::కూడు_inner(Twine_కణుపు t) {
	ఒకవేళ (free_list.ఖాళీలు() && STATIC_లెక్క + backing.కొలత() > IdString::MAX_సూచీ)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_సూచీ - STATIC_లెక్క);
	తిరిగి HashConsPool::కూడు_inner(std::move(t));
}

శూన్య TwinePool::dump(IdString ref, std::ostream& os) స్థిరరాశి {
	స్థిరరాశి Twine_కణుపు& twine = (*ఇది)[ref];
	ఎంపిక (twine.రకం()) {
	స్థితి Twine_కణుపు::రకం_2::Dead:
		os << "Dead()";
		ఆపు;
	స్థితి Twine_కణుపు::రకం_2::ఆకు:
		os << "Leaf(\"" << twine.పాఠ్యం() << "\")";
		ఆపు;
	స్థితి Twine_కణుపు::రకం_2::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.ఉపసర్గ_2(), os);
		os << ", tail: \"" << twine.పాఠ్యం() << "\")";
		ఆపు;
	}
	ఒకవేళ (ref.is_ప్రజా())
		os << " pub";
}

శూన్య TwinePool::append_str(IdString ref, std::string& బహిర్గతం) స్థిరరాశి {
	ఒకవేళ (ref == IdString::Null)
		తిరిగి;
	ఒకవేళ (ref.is_ప్రజా())
		బహిర్గతం += '\\';
	స్థిరరాశి Twine_కణుపు& twine = (*ఇది)[ref];
	ఎంపిక (twine.రకం()) {
	స్థితి Twine_కణుపు::రకం_2::Dead:
		ఆపు;
	స్థితి Twine_కణుపు::రకం_2::Suffix:
		append_str(twine.ఉపసర్గ_2(), బహిర్గతం);
		[[fallthrough]];
	స్థితి Twine_కణుపు::రకం_2::ఆకు:
		బహిర్గతం += twine.పాఠ్యం();
		ఆపు;
	}
}

std::string TwinePool::str(IdString ref) స్థిరరాశి {
	std::string బహిర్గతం;
	append_str(ref, బహిర్గతం);
	తిరిగి బహిర్గతం;
}

std::string TwinePool::unescaped_str(IdString ref) స్థిరరాశి {
	తిరిగి str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t ఉపసర్గ_2, std::string_view పాఠ్యం) స్థిరరాశి {
	uint32_t hash = ఉపసర్గ_2 == Twine_కణుపు::NO_ఉపసర్గ ? 0 : (*ఇది)[IdString(ఉపసర్గ_2)].hash();
	తిరిగి Twine_కణుపు::extend_hash(hash, పాఠ్యం);
}

IdString TwinePool::కనుగొను_content(uint32_t ఉపసర్గ_2, std::string_view పాఠ్యం) స్థిరరాశి {
	తిరిగి కనుగొను_content(ఉపసర్గ_2, పాఠ్యం, content_hash(ఉపసర్గ_2, పాఠ్యం));
}

IdString TwinePool::intern(uint32_t ఉపసర్గ_2, std::string_view పాఠ్యం) {
	log_assert(!Multithreading::చురుకైన());
	uint32_t hash = content_hash(ఉపసర్గ_2, పాఠ్యం);
	IdString ref = కనుగొను_content(ఉపసర్గ_2, పాఠ్యం, hash);
	ఒకవేళ (ref != IdString::Null)
		తిరిగి ref;
	తిరిగి కూడు_inner(Twine_కణుపు{ఉపసర్గ_2, పాఠ్యం, hash});
}

IdString TwinePool::కనుగొను(std::string_view name) స్థిరరాశి {
	ఒకవేళ (name.ఖాళీలు())
		తిరిగి IdString::Null;
	check_name(name);
	స్వయంచాలక [content, is_ప్రజా_2] = unescape_name(name);
	తిరిగి stamp(కనుగొను_content(Twine_కణుపు::NO_ఉపసర్గ, content).ట్యాగ్(is_ప్రజా_2));
}

IdString TwinePool::కనుగొను(TwineSpec t) స్థిరరాశి {
	ఒకవేళ (std::holds_alternative<TwineSpec::AutoSuffix>(t.దత్తాంశం))
		తిరిగి stamp(కనుగొను_content(Twine_కణుపు::NO_ఉపసర్గ, t.content_str()));
	ఒకవేళ (స్వయంచాలక *ఆకు_2 = std::get_if<TwineSpec::ఆకు>(&t.దత్తాంశం))
		తిరిగి stamp(కనుగొను_content(Twine_కణుపు::NO_ఉపసర్గ, ఆకు_2->s));
	స్థిరరాశి TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.దత్తాంశం);
	తిరిగి stamp(కనుగొను_content((uint32_t)sfx.ఉపసర్గ_2.untag().raw(), sfx.తోక).ట్యాగ్(sfx.ఉపసర్గ_2.is_ప్రజా()));
}

IdString TwinePool::కూడు(TwineSpec t) {
	ఒకవేళ (స్వయంచాలక *ap = std::get_if<TwineSpec::AutoSuffix>(&t.దత్తాంశం))
		తిరిగి కూడు(auto_ఉపసర్గ(ap->ఉపసర్గ_2), ap->తోక);
	ఒకవేళ (స్వయంచాలక *ఆకు_2 = std::get_if<TwineSpec::ఆకు>(&t.దత్తాంశం))
		తిరిగి stamp(intern(Twine_కణుపు::NO_ఉపసర్గ, ఆకు_2->s));
	స్థిరరాశి TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.దత్తాంశం);
	తిరిగి కూడు(sfx.ఉపసర్గ_2, sfx.తోక);
}

IdString TwinePool::auto_ఉపసర్గ(స్థిరరాశి std::string *ఉపసర్గ_2) {
	స్వయంచాలక it = auto_prefixes.కనుగొను(ఉపసర్గ_2);
	ఒకవేళ (it != auto_prefixes.చివర())
		తిరిగి it->రెండవ;
	IdString ref = intern(Twine_కణుపు::NO_ఉపసర్గ, *ఉపసర్గ_2);
	auto_prefixes[ఉపసర్గ_2] = ref;
	తిరిగి ref;
}

IdString TwinePool::కూడు(IdString ఉపసర్గ_2, std::string_view తోక) {
	తిరిగి stamp(intern((uint32_t)ఉపసర్గ_2.untag().raw(), తోక).ట్యాగ్(ఉపసర్గ_2.is_ప్రజా()));
}

IdString TwinePool::కూడు(std::string_view s) {
	ఒకవేళ (s.ఖాళీలు())
		తిరిగి IdString::Null;
	check_name(s);
	స్వయంచాలక [content, is_ప్రజా_2] = unescape_name(s);
	తిరిగి stamp(intern(Twine_కణుపు::NO_ఉపసర్గ, content).ట్యాగ్(is_ప్రజా_2));
}

IdString TwinePool::copy_from(స్థిరరాశి TwinePool& src, IdString ref) {
	IdString దొరికింది = కనుగొను_from(src, ref);
	ఒకవేళ (దొరికింది != IdString::Null || ref == IdString::Null)
		తిరిగి దొరికింది;
	స్థిరరాశి Twine_కణుపు& t = src[ref.untag()];
	ఒకవేళ (t.is_dead())
		తిరిగి IdString::Null;
	uint32_t ఉపసర్గ_2 = Twine_కణుపు::NO_ఉపసర్గ;
	ఒకవేళ (t.is_suffix())
		ఉపసర్గ_2 = (uint32_t)copy_from(src, t.ఉపసర్గ_2()).untag().raw();
	తిరిగి stamp(intern(ఉపసర్గ_2, t.పాఠ్యం()).ట్యాగ్(ref.is_ప్రజా()));
}

std::string TwinePool::ref_token(IdString ref) స్థిరరాశి {
	తిరిగి "#" + std::గమ్యం_string((uint64_t)serial_) + ":"
		+ std::గమ్యం_string((uint64_t)stamp(ref).బిట్లు());
}

IdString TwinePool::ref_from_token(std::string_view token) స్థిరరాశి {
	ఒకవేళ (token.కొలత() < 2 || token[0] != '#')
		తిరిగి IdString::Null;
	కొలత_t sep = token.కనుగొను(':');
	ఒకవేళ (sep == std::string_view::npos)
		తిరిగి IdString::Null;
	కొలత_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	కొరకు (పూర్ణాంకం i = 0; i < 2; i++) {
		ఒకవేళ (parts[i].ఖాళీలు())
			తిరిగి IdString::Null;
		కొరకు (అక్షరం c : parts[i]) {
			ఒకవేళ (c < '0' || c > '9')
				తిరిగి IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	ఒకవేళ (fields[0] != serial_)
		తిరిగి IdString::Null;
	IdString ref(fields[1]);
	ఒకవేళ (ref == IdString::Null)
		తిరిగి IdString::Null;
	తిరిగి is_సజీవ(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
నిర్మాణం TwineSegments {
	స్థిర constexpr కొలత_t SEGMENT_INLINE_లోతు = 8;
	TwineSegments(స్థిరరాశి TwinePool &pool, IdString ref);
	TwineSegments(స్థిరరాశి TwinePool &pool, IdString తల, std::string_view తోక);
	std::string_view peek();
	శూన్య advance(కొలత_t n);
	కొలత_t మొత్తం_కొలత() స్థిరరాశి;
అంతరంగ:
	శూన్య నెట్టు(std::string_view seg);
	std::string_view *segs();
	స్థిరరాశి std::string_view *segs() స్థిరరాశి;
	std::string_view inline_segs[SEGMENT_INLINE_లోతు];
	std::సదిశ<std::string_view> spill;
	కొలత_t లెక్క = 0;
	కొలత_t pos = 0;
	కొలత_t off = 0;
};

TwineSegments::TwineSegments(స్థిరరాశి TwinePool &pool, IdString ref)
{
	ఒకవేళ (ref == IdString::Null)
		తిరిగి;
	కొరకు (IdString cur = ref.untag(); ;) {
		స్థిరరాశి Twine_కణుపు &t = pool[cur];
		ఎంపిక (t.రకం()) {
		స్థితి Twine_కణుపు::రకం_2::Suffix:
			నెట్టు(t.పాఠ్యం());
			cur = t.ఉపసర్గ_2();
			కొనసాగు;
		స్థితి Twine_కణుపు::రకం_2::ఆకు:
			నెట్టు(t.పాఠ్యం());
			ఆపు;
		స్థితి Twine_కణుపు::రకం_2::Dead:
			ఆపు;
		}
		ఆపు;
	}
	ఒకవేళ (ref.is_ప్రజా())
		నెట్టు("\\");
	std::తిరగేయి(segs(), segs() + లెక్క);
}

TwineSegments::TwineSegments(స్థిరరాశి TwinePool &pool, IdString తల, std::string_view తోక)
	: TwineSegments(pool, తల)
{
	నెట్టు(తోక);
}

std::string_view TwineSegments::peek()
{
	అయితే (pos < లెక్క) {
		std::string_view seg = segs()[pos];
		ఒకవేళ (off < seg.కొలత())
			తిరిగి seg.substr(off);
		pos++;
		off = 0;
	}
	తిరిగి {};
}

శూన్య TwineSegments::advance(కొలత_t n) { off += n; }

కొలత_t TwineSegments::మొత్తం_కొలత() స్థిరరాశి
{
	కొలత_t మొత్తం = 0;
	కొరకు (కొలత_t i = 0; i < లెక్క; i++)
		మొత్తం += segs()[i].కొలత();
	తిరిగి మొత్తం;
}

శూన్య TwineSegments::నెట్టు(std::string_view seg)
{
	ఒకవేళ (spill.ఖాళీలు() && లెక్క < SEGMENT_INLINE_లోతు) {
		inline_segs[లెక్క++] = seg;
		తిరిగి;
	}
	ఒకవేళ (spill.ఖాళీలు())
		spill.కేటాయించు(inline_segs, inline_segs + లెక్క);
	spill.నెట్టు_వెనుక(seg);
	లెక్క++;
}

std::string_view *TwineSegments::segs() { తిరిగి spill.ఖాళీలు() ? inline_segs : spill.దత్తాంశం(); }
స్థిరరాశి std::string_view *TwineSegments::segs() స్థిరరాశి { తిరిగి spill.ఖాళీలు() ? inline_segs : spill.దత్తాంశం(); }

కొలత_t TwinePool::str_కొలత(IdString ref) స్థిరరాశి
{
	తిరిగి TwineSegments(*ఇది, ref).మొత్తం_కొలత();
}

తార్కిక TwinePool::begins_with(IdString ref, std::string_view ఉపసర్గ_2) స్థిరరాశి
{
	TwineSegments segs(*ఇది, ref);
	అయితే (!ఉపసర్గ_2.ఖాళీలు()) {
		std::string_view seg = segs.peek();
		ఒకవేళ (seg.ఖాళీలు())
			తిరిగి అబద్ధం;
		కొలత_t n = std::min(seg.కొలత(), ఉపసర్గ_2.కొలత());
		ఒకవేళ (std::memcmp(seg.దత్తాంశం(), ఉపసర్గ_2.దత్తాంశం(), n) != 0)
			తిరిగి అబద్ధం;
		segs.advance(n);
		ఉపసర్గ_2.remove_ఉపసర్గ(n);
	}
	తిరిగి నిజం;
}

తార్కిక TwinePool::name_equal(IdString ref, std::string_view name) స్థిరరాశి
{
	తిరిగి str_కొలత(ref) == name.కొలత() && begins_with(ref, name);
}

స్థిర పూర్ణాంకం compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	అయితే (నిజం) {
		std::string_view x = sa.peek(), y = sb.peek();
		ఒకవేళ (x.ఖాళీలు() || y.ఖాళీలు())
			తిరిగి x.ఖాళీలు() ? (y.ఖాళీలు() ? 0 : -1) : 1;
		కొలత_t n = std::min(x.కొలత(), y.కొలత());
		ఒకవేళ (పూర్ణాంకం diff = std::memcmp(x.దత్తాంశం(), y.దత్తాంశం(), n); diff != 0)
			తిరిగి diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::కనుగొను_content(uint32_t ఉపసర్గ_2, std::string_view పాఠ్యం, uint32_t hash) స్థిరరాశి {
	తిరిగి కనుగొను_hashed(hash, [&](IdString ref) {
		స్థిరరాశి Twine_కణుపు &t = (*ఇది)[ref];
		ఒకవేళ (t == Twine_కణుపు::కీలకం_2{ఉపసర్గ_2, పాఠ్యం})
			తిరిగి నిజం;
		IdString తల = ఉపసర్గ_2 == Twine_కణుపు::NO_ఉపసర్గ ? IdString::Null : IdString(ఉపసర్గ_2);
		TwineSegments sa(*ఇది, ref), sb(*ఇది, తల, పాఠ్యం);
		తిరిగి compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::కనుగొను_from(స్థిరరాశి TwinePool& src, IdString ref) స్థిరరాశి {
	ఒకవేళ (ref == IdString::Null || ID::is_static(ref))
		తిరిగి ref;
	IdString idx = ref.untag();
	స్థిరరాశి Twine_కణుపు& t = src[idx];
	ఒకవేళ (t.is_dead())
		తిరిగి IdString::Null;
	IdString దొరికింది = కనుగొను_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*ఇది, cand), sb(src, idx);
		తిరిగి compare_segments(sa, sb) == 0;
	});
	తిరిగి stamp(దొరికింది.ట్యాగ్(ref.is_ప్రజా()));
}

IdString TwinePool::ఉపసర్గ_యొక్క(IdString ref) స్థిరరాశి {
	స్థిరరాశి Twine_కణుపు &t = (*ఇది)[ref];
	ఒకవేళ (!t.is_suffix())
		తిరిగి IdString::Null;
	తిరిగి stamp(t.ఉపసర్గ_2().ట్యాగ్(ref.is_ప్రజా()));
}

పూర్ణాంకం TwinePool::compare_by_name(IdString a, IdString b) స్థిరరాశి
{
	ఒకవేళ (a == b)
		తిరిగి 0;
	ఒకవేళ (a == IdString::Null)
		తిరిగి -1;
	ఒకవేళ (b == IdString::Null)
		తిరిగి 1;

	ఒకవేళ (a.is_ప్రజా() == b.is_ప్రజా()) {
		స్థిరరాశి Twine_కణుపు &ta = (*ఇది)[a.untag()];
		స్థిరరాశి Twine_కణుపు &tb = (*ఇది)[b.untag()];
		ఒకవేళ (ta.is_ఆకు() && tb.is_ఆకు())
			తిరిగి ta.పాఠ్యం().compare(tb.పాఠ్యం());
		ఒకవేళ (ta.is_suffix() && tb.is_suffix() && ta.ఉపసర్గ_2() == tb.ఉపసర్గ_2())
			తిరిగి ta.పాఠ్యం().compare(tb.పాఠ్యం());
	}

	TwineSegments sa(*ఇది, a), sb(*ఇది, b);
	తిరిగి compare_segments(sa, sb);
}

YOSYS_NAMESPACE_చివర
