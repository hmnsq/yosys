#ضمن "kernel/twine.h"
#ضمن "kernel/hashcons_impl.h"
#ضمن "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

سانچو بنية HashConsPool<TwinePool, Twine_ڳنڍ, IdString>;

std::ويڪٽر<Twine_ڳنڍ> StaticTwines::nodes_;

فراغ StaticTwines::init() {
	إذا (ready())
		أرجع;
	log_assert(nodes_.خالي_جايون());
	nodes_.reserve(STATIC_TWINE_پڇاڙي);
	لكل (ثابت حرف *name : ID::static_نالا)
		nodes_.emplace_پٺ(std::string_view(name));
}

ثابت Twine_ڳنڍ &StaticTwines::ڳنڍ(سائيز_t idx) { أرجع nodes_[idx]; }
منطقي StaticTwines::ready() { أرجع nodes_.سائيز() == STATIC_TWINE_پڇاڙي; }

int64_t twine_gc_ns;
صحيح twine_gc_ڳڻپ;

std::string IdString::handle_token() ثابت {
	أرجع stringf("%s@%zu", is_عوامي() ? "$pub" : "$priv", untag().raw());
}

سائيز_t IdString::handle_token_اڳياڙي(std::string_view token, منطقي &is_عوامي_2) {
	إذا (token.substr(0, 5) == "$pub@") {
		is_عوامي_2 = سچ;
		أرجع 5;
	}
	إذا (token.substr(0, 6) == "$priv@") {
		is_عوامي_2 = خطأ;
		أرجع 6;
	}
	أرجع 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_پڇاڙي);
	std::string نتيجو = ref.is_عوامي() ? "\\" : "";
	نتيجو += static_نالا[idx.raw()];
	أرجع نتيجو;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_پڇاڙي);
	أرجع static_نالا[idx.raw()];
}

TwineSpec::TwineSpec(پن v) : معلومات(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : معلومات(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : معلومات(std::move(v)) {}

فراغ SmallString::store(std::string_view content)
{
	log_assert(content.سائيز() <= MAX_LEN);
	release();
	len_ = content.سائيز();
	إذا (len_ > INLINE_CAP) {
		ptr_ = جديد حرف[len_];
		يادگيري_نقل(ptr_, content.معلومات(), len_);
	} وإلا {
		يادگيري_نقل(inl_, content.معلومات(), len_);
	}
}

فراغ SmallString::release()
{
	إذا (len_ > INLINE_CAP)
		ميٽيو[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(ثابت SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	يادگيري_نقل(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::عامل=(ثابت SmallString &other)
{
	إذا (هذا != &other)
		store(other.view());
	أرجع *هذا;
}

SmallString &SmallString::عامل=(SmallString &&other) noexcept
{
	إذا (هذا == &other)
		أرجع *هذا;
	release();
	يادگيري_نقل(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	أرجع *هذا;
}

SmallString::~SmallString() { release(); }

Twine_ڳنڍ::Twine_ڳنڍ(Twine_ڳنڍ &&other) noexcept
	: متن_2(std::move(other.متن_2)), اڳياڙي_3(other.اڳياڙي_3), hash_(other.hash_)
{
	other.اڳياڙي_3 = DEAD;
}

Twine_ڳنڍ &Twine_ڳنڍ::عامل=(Twine_ڳنڍ &&other) noexcept
{
	إذا (هذا == &other)
		أرجع *هذا;
	متن_2 = std::move(other.متن_2);
	اڳياڙي_3 = other.اڳياڙي_3;
	hash_ = other.hash_;
	other.اڳياڙي_3 = DEAD;
	أرجع *هذا;
}

std::string TwineSpec::content_str() ثابت {
	إذا (خودڪار *پن_2 = std::get_if<پن>(&معلومات))
		أرجع پن_2->s;
	خودڪار &autosfx = std::get<AutoSuffix>(معلومات);
	أرجع *autosfx.اڳياڙي_2 + autosfx.پڇ;
}

ساكن std::pair<std::string_view, منطقي> unescape_name(std::string_view name) {
	إذا (!name.خالي_جايون() && name[0] == '\\')
		أرجع {name.substr(1), سچ};
	أرجع {name, خطأ};
}

ساكن فراغ check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	لكل (حرف ch : name)
		إذا ((بنانشان)ch <= (بنانشان)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(ايندڙ_serial()) {}
TwinePool::TwinePool(ثابت TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(ايندڙ_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(ايندڙ_serial()) {}

TwinePool& TwinePool::عامل=(ثابت TwinePool& other) {
	إذا (هذا == &other)
		أرجع *هذا;
	HashConsPool::عامل=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = ايندڙ_serial();
	أرجع *هذا;
}

TwinePool& TwinePool::عامل=(TwinePool&& other) {
	إذا (هذا == &other)
		أرجع *هذا;
	HashConsPool::عامل=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = ايندڙ_serial();
	أرجع *هذا;
}

منطقي TwinePool::owns(IdString ref) ثابت {
	أرجع ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ثابت {
	إذا (ref == IdString::Null || ID::is_static(ref))
		أرجع ref;
	أرجع ref.stamped(serial_);
}

فراغ TwinePool::check_owned(IdString ref) ثابت {
	إذا constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ثابت Twine_ڳنڍ& TwinePool::عامل[](IdString ref) ثابت {
	check_owned(ref);
	أرجع HashConsPool::عامل[](ref);
}

سائيز_t TwinePool::gc(pool<IdString>& roots) {
	لكل (خودڪار &it : auto_prefixes)
		roots.وجهو(it.ٻيو);
	أرجع HashConsPool::gc(roots);
}

ثابت Twine_ڳنڍ& TwinePool::static_ڳنڍ(سائيز_t idx) { أرجع StaticTwines::ڳنڍ(idx); }
فراغ TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

سائيز_t TwinePool::ايندڙ_serial() {
	ساكن سائيز_t counter = 0;
	سائيز_t serial = ++counter;
	إذا constexpr (IdString::MAX_SERIAL == 0)
		أرجع serial;
	وإلا
		أرجع serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::جوڙيو_inner(Twine_ڳنڍ t) {
	إذا (free_list.خالي_جايون() && STATIC_ڳڻپ + backing.سائيز() > IdString::MAX_اشاريو)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_اشاريو - STATIC_ڳڻپ);
	أرجع HashConsPool::جوڙيو_inner(std::move(t));
}

فراغ TwinePool::dump(IdString ref, std::ostream& os) ثابت {
	ثابت Twine_ڳنڍ& twine = (*هذا)[ref];
	بدل (twine.قسم()) {
	حالة Twine_ڳنڍ::قسم_2::Dead:
		os << "Dead()";
		اكسر;
	حالة Twine_ڳنڍ::قسم_2::پن:
		os << "Leaf(\"" << twine.متن() << "\")";
		اكسر;
	حالة Twine_ڳنڍ::قسم_2::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.اڳياڙي_2(), os);
		os << ", tail: \"" << twine.متن() << "\")";
		اكسر;
	}
	إذا (ref.is_عوامي())
		os << " pub";
}

فراغ TwinePool::append_str(IdString ref, std::string& ٻاهريون) ثابت {
	إذا (ref == IdString::Null)
		أرجع;
	إذا (ref.is_عوامي())
		ٻاهريون += '\\';
	ثابت Twine_ڳنڍ& twine = (*هذا)[ref];
	بدل (twine.قسم()) {
	حالة Twine_ڳنڍ::قسم_2::Dead:
		اكسر;
	حالة Twine_ڳنڍ::قسم_2::Suffix:
		append_str(twine.اڳياڙي_2(), ٻاهريون);
		[[fallthrough]];
	حالة Twine_ڳنڍ::قسم_2::پن:
		ٻاهريون += twine.متن();
		اكسر;
	}
}

std::string TwinePool::str(IdString ref) ثابت {
	std::string ٻاهريون;
	append_str(ref, ٻاهريون);
	أرجع ٻاهريون;
}

std::string TwinePool::unescaped_str(IdString ref) ثابت {
	أرجع str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t اڳياڙي_2, std::string_view متن) ثابت {
	uint32_t hash = اڳياڙي_2 == Twine_ڳنڍ::NO_اڳياڙي ? 0 : (*هذا)[IdString(اڳياڙي_2)].hash();
	أرجع Twine_ڳنڍ::extend_hash(hash, متن);
}

IdString TwinePool::ڳوليو_content(uint32_t اڳياڙي_2, std::string_view متن) ثابت {
	أرجع ڳوليو_content(اڳياڙي_2, متن, content_hash(اڳياڙي_2, متن));
}

IdString TwinePool::intern(uint32_t اڳياڙي_2, std::string_view متن) {
	log_assert(!Multithreading::سرگرم());
	uint32_t hash = content_hash(اڳياڙي_2, متن);
	IdString ref = ڳوليو_content(اڳياڙي_2, متن, hash);
	إذا (ref != IdString::Null)
		أرجع ref;
	أرجع جوڙيو_inner(Twine_ڳنڍ{اڳياڙي_2, متن, hash});
}

IdString TwinePool::ڳوليو(std::string_view name) ثابت {
	إذا (name.خالي_جايون())
		أرجع IdString::Null;
	check_name(name);
	خودڪار [content, is_عوامي_2] = unescape_name(name);
	أرجع stamp(ڳوليو_content(Twine_ڳنڍ::NO_اڳياڙي, content).ٽيگ(is_عوامي_2));
}

IdString TwinePool::ڳوليو(TwineSpec t) ثابت {
	إذا (std::holds_alternative<TwineSpec::AutoSuffix>(t.معلومات))
		أرجع stamp(ڳوليو_content(Twine_ڳنڍ::NO_اڳياڙي, t.content_str()));
	إذا (خودڪار *پن_2 = std::get_if<TwineSpec::پن>(&t.معلومات))
		أرجع stamp(ڳوليو_content(Twine_ڳنڍ::NO_اڳياڙي, پن_2->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.معلومات);
	أرجع stamp(ڳوليو_content((uint32_t)sfx.اڳياڙي_2.untag().raw(), sfx.پڇ).ٽيگ(sfx.اڳياڙي_2.is_عوامي()));
}

IdString TwinePool::جوڙيو(TwineSpec t) {
	إذا (خودڪار *ap = std::get_if<TwineSpec::AutoSuffix>(&t.معلومات))
		أرجع جوڙيو(auto_اڳياڙي(ap->اڳياڙي_2), ap->پڇ);
	إذا (خودڪار *پن_2 = std::get_if<TwineSpec::پن>(&t.معلومات))
		أرجع stamp(intern(Twine_ڳنڍ::NO_اڳياڙي, پن_2->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.معلومات);
	أرجع جوڙيو(sfx.اڳياڙي_2, sfx.پڇ);
}

IdString TwinePool::auto_اڳياڙي(ثابت std::string *اڳياڙي_2) {
	خودڪار it = auto_prefixes.ڳوليو(اڳياڙي_2);
	إذا (it != auto_prefixes.پڇاڙي())
		أرجع it->ٻيو;
	IdString ref = intern(Twine_ڳنڍ::NO_اڳياڙي, *اڳياڙي_2);
	auto_prefixes[اڳياڙي_2] = ref;
	أرجع ref;
}

IdString TwinePool::جوڙيو(IdString اڳياڙي_2, std::string_view پڇ) {
	أرجع stamp(intern((uint32_t)اڳياڙي_2.untag().raw(), پڇ).ٽيگ(اڳياڙي_2.is_عوامي()));
}

IdString TwinePool::جوڙيو(std::string_view s) {
	إذا (s.خالي_جايون())
		أرجع IdString::Null;
	check_name(s);
	خودڪار [content, is_عوامي_2] = unescape_name(s);
	أرجع stamp(intern(Twine_ڳنڍ::NO_اڳياڙي, content).ٽيگ(is_عوامي_2));
}

IdString TwinePool::copy_from(ثابت TwinePool& src, IdString ref) {
	IdString مليو = ڳوليو_from(src, ref);
	إذا (مليو != IdString::Null || ref == IdString::Null)
		أرجع مليو;
	ثابت Twine_ڳنڍ& t = src[ref.untag()];
	إذا (t.is_dead())
		أرجع IdString::Null;
	uint32_t اڳياڙي_2 = Twine_ڳنڍ::NO_اڳياڙي;
	إذا (t.is_suffix())
		اڳياڙي_2 = (uint32_t)copy_from(src, t.اڳياڙي_2()).untag().raw();
	أرجع stamp(intern(اڳياڙي_2, t.متن()).ٽيگ(ref.is_عوامي()));
}

std::string TwinePool::ref_token(IdString ref) ثابت {
	أرجع "#" + std::منزل_string((uint64_t)serial_) + ":"
		+ std::منزل_string((uint64_t)stamp(ref).بٽ());
}

IdString TwinePool::ref_from_token(std::string_view token) ثابت {
	إذا (token.سائيز() < 2 || token[0] != '#')
		أرجع IdString::Null;
	سائيز_t sep = token.ڳوليو(':');
	إذا (sep == std::string_view::npos)
		أرجع IdString::Null;
	سائيز_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	لكل (صحيح i = 0; i < 2; i++) {
		إذا (parts[i].خالي_جايون())
			أرجع IdString::Null;
		لكل (حرف c : parts[i]) {
			إذا (c < '0' || c > '9')
				أرجع IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	إذا (fields[0] != serial_)
		أرجع IdString::Null;
	IdString ref(fields[1]);
	إذا (ref == IdString::Null)
		أرجع IdString::Null;
	أرجع is_جيئرو(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
بنية TwineSegments {
	ساكن constexpr سائيز_t SEGMENT_INLINE_اونهائي = 8;
	TwineSegments(ثابت TwinePool &pool, IdString ref);
	TwineSegments(ثابت TwinePool &pool, IdString سر, std::string_view پڇ);
	std::string_view peek();
	فراغ advance(سائيز_t n);
	سائيز_t ڪل_سائيز() ثابت;
خاص:
	فراغ ڌڪو(std::string_view seg);
	std::string_view *segs();
	ثابت std::string_view *segs() ثابت;
	std::string_view inline_segs[SEGMENT_INLINE_اونهائي];
	std::ويڪٽر<std::string_view> spill;
	سائيز_t ڳڻپ = 0;
	سائيز_t pos = 0;
	سائيز_t off = 0;
};

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString ref)
{
	إذا (ref == IdString::Null)
		أرجع;
	لكل (IdString cur = ref.untag(); ;) {
		ثابت Twine_ڳنڍ &t = pool[cur];
		بدل (t.قسم()) {
		حالة Twine_ڳنڍ::قسم_2::Suffix:
			ڌڪو(t.متن());
			cur = t.اڳياڙي_2();
			استمر;
		حالة Twine_ڳنڍ::قسم_2::پن:
			ڌڪو(t.متن());
			اكسر;
		حالة Twine_ڳنڍ::قسم_2::Dead:
			اكسر;
		}
		اكسر;
	}
	إذا (ref.is_عوامي())
		ڌڪو("\\");
	std::ابتو(segs(), segs() + ڳڻپ);
}

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString سر, std::string_view پڇ)
	: TwineSegments(pool, سر)
{
	ڌڪو(پڇ);
}

std::string_view TwineSegments::peek()
{
	طالما (pos < ڳڻپ) {
		std::string_view seg = segs()[pos];
		إذا (off < seg.سائيز())
			أرجع seg.substr(off);
		pos++;
		off = 0;
	}
	أرجع {};
}

فراغ TwineSegments::advance(سائيز_t n) { off += n; }

سائيز_t TwineSegments::ڪل_سائيز() ثابت
{
	سائيز_t ڪل = 0;
	لكل (سائيز_t i = 0; i < ڳڻپ; i++)
		ڪل += segs()[i].سائيز();
	أرجع ڪل;
}

فراغ TwineSegments::ڌڪو(std::string_view seg)
{
	إذا (spill.خالي_جايون() && ڳڻپ < SEGMENT_INLINE_اونهائي) {
		inline_segs[ڳڻپ++] = seg;
		أرجع;
	}
	إذا (spill.خالي_جايون())
		spill.سونپيو(inline_segs, inline_segs + ڳڻپ);
	spill.ڌڪو_پٺ(seg);
	ڳڻپ++;
}

std::string_view *TwineSegments::segs() { أرجع spill.خالي_جايون() ? inline_segs : spill.معلومات(); }
ثابت std::string_view *TwineSegments::segs() ثابت { أرجع spill.خالي_جايون() ? inline_segs : spill.معلومات(); }

سائيز_t TwinePool::str_سائيز(IdString ref) ثابت
{
	أرجع TwineSegments(*هذا, ref).ڪل_سائيز();
}

منطقي TwinePool::begins_with(IdString ref, std::string_view اڳياڙي_2) ثابت
{
	TwineSegments segs(*هذا, ref);
	طالما (!اڳياڙي_2.خالي_جايون()) {
		std::string_view seg = segs.peek();
		إذا (seg.خالي_جايون())
			أرجع خطأ;
		سائيز_t n = std::min(seg.سائيز(), اڳياڙي_2.سائيز());
		إذا (std::memcmp(seg.معلومات(), اڳياڙي_2.معلومات(), n) != 0)
			أرجع خطأ;
		segs.advance(n);
		اڳياڙي_2.remove_اڳياڙي(n);
	}
	أرجع سچ;
}

منطقي TwinePool::name_equal(IdString ref, std::string_view name) ثابت
{
	أرجع str_سائيز(ref) == name.سائيز() && begins_with(ref, name);
}

ساكن صحيح compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	طالما (سچ) {
		std::string_view x = sa.peek(), y = sb.peek();
		إذا (x.خالي_جايون() || y.خالي_جايون())
			أرجع x.خالي_جايون() ? (y.خالي_جايون() ? 0 : -1) : 1;
		سائيز_t n = std::min(x.سائيز(), y.سائيز());
		إذا (صحيح diff = std::memcmp(x.معلومات(), y.معلومات(), n); diff != 0)
			أرجع diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::ڳوليو_content(uint32_t اڳياڙي_2, std::string_view متن, uint32_t hash) ثابت {
	أرجع ڳوليو_hashed(hash, [&](IdString ref) {
		ثابت Twine_ڳنڍ &t = (*هذا)[ref];
		إذا (t == Twine_ڳنڍ::ڪنجي_2{اڳياڙي_2, متن})
			أرجع سچ;
		IdString سر = اڳياڙي_2 == Twine_ڳنڍ::NO_اڳياڙي ? IdString::Null : IdString(اڳياڙي_2);
		TwineSegments sa(*هذا, ref), sb(*هذا, سر, متن);
		أرجع compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::ڳوليو_from(ثابت TwinePool& src, IdString ref) ثابت {
	إذا (ref == IdString::Null || ID::is_static(ref))
		أرجع ref;
	IdString idx = ref.untag();
	ثابت Twine_ڳنڍ& t = src[idx];
	إذا (t.is_dead())
		أرجع IdString::Null;
	IdString مليو = ڳوليو_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*هذا, cand), sb(src, idx);
		أرجع compare_segments(sa, sb) == 0;
	});
	أرجع stamp(مليو.ٽيگ(ref.is_عوامي()));
}

IdString TwinePool::اڳياڙي_جو(IdString ref) ثابت {
	ثابت Twine_ڳنڍ &t = (*هذا)[ref];
	إذا (!t.is_suffix())
		أرجع IdString::Null;
	أرجع stamp(t.اڳياڙي_2().ٽيگ(ref.is_عوامي()));
}

صحيح TwinePool::compare_by_name(IdString a, IdString b) ثابت
{
	إذا (a == b)
		أرجع 0;
	إذا (a == IdString::Null)
		أرجع -1;
	إذا (b == IdString::Null)
		أرجع 1;

	إذا (a.is_عوامي() == b.is_عوامي()) {
		ثابت Twine_ڳنڍ &ta = (*هذا)[a.untag()];
		ثابت Twine_ڳنڍ &tb = (*هذا)[b.untag()];
		إذا (ta.is_پن() && tb.is_پن())
			أرجع ta.متن().compare(tb.متن());
		إذا (ta.is_suffix() && tb.is_suffix() && ta.اڳياڙي_2() == tb.اڳياڙي_2())
			أرجع ta.متن().compare(tb.متن());
	}

	TwineSegments sa(*هذا, a), sb(*هذا, b);
	أرجع compare_segments(sa, sb);
}

YOSYS_NAMESPACE_پڇاڙي
