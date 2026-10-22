#अन्तर्भाव "kernel/twine.h"
#अन्तर्भाव "kernel/hashcons_impl.h"
#अन्तर्भाव "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

प्रतिरूपम् संरचना HashConsPool<TwinePool, Twine_ग्रन्थिः, IdString>;

std::सदिशम्<Twine_ग्रन्थिः> StaticTwines::nodes_;

शून्य StaticTwines::init() {
	यदि (ready())
		प्रत्यागम;
	log_assert(nodes_.रिक्तानि());
	nodes_.reserve(STATIC_TWINE_अन्तः);
	प्रति (ध्रुव वर्ण *name : ID::static_नामानि)
		nodes_.emplace_पृष्ठभागः(std::string_view(name));
}

ध्रुव Twine_ग्रन्थिः &StaticTwines::ग्रन्थिः(आकार_t idx) { प्रत्यागम nodes_[idx]; }
तार्किक StaticTwines::ready() { प्रत्यागम nodes_.आकार() == STATIC_TWINE_अन्तः; }

int64_t twine_gc_ns;
पूर्णाङ्क twine_gc_गणनम्;

std::string IdString::handle_token() ध्रुव {
	प्रत्यागम stringf("%s@%zu", is_लौकिकम्() ? "$pub" : "$priv", untag().raw());
}

आकार_t IdString::handle_token_उपसर्गः(std::string_view token, तार्किक &is_लौकिकम्_2) {
	यदि (token.substr(0, 5) == "$pub@") {
		is_लौकिकम्_2 = सत्य;
		प्रत्यागम 5;
	}
	यदि (token.substr(0, 6) == "$priv@") {
		is_लौकिकम्_2 = असत्य;
		प्रत्यागम 6;
	}
	प्रत्यागम 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_अन्तः);
	std::string परिणाम = ref.is_लौकिकम्() ? "\\" : "";
	परिणाम += static_नामानि[idx.raw()];
	प्रत्यागम परिणाम;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_अन्तः);
	प्रत्यागम static_नामानि[idx.raw()];
}

TwineSpec::TwineSpec(पर्णम् v) : दत्त(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : दत्त(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : दत्त(std::move(v)) {}

शून्य SmallString::store(std::string_view content)
{
	log_assert(content.आकार() <= MAX_LEN);
	release();
	len_ = content.आकार();
	यदि (len_ > INLINE_CAP) {
		ptr_ = नूतन वर्ण[len_];
		स्मृतिं_प्रतिलिख(ptr_, content.दत्त(), len_);
	} अन्यथा {
		स्मृतिं_प्रतिलिख(inl_, content.दत्त(), len_);
	}
}

शून्य SmallString::release()
{
	यदि (len_ > INLINE_CAP)
		विलोपय[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(ध्रुव SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	स्मृतिं_प्रतिलिख(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::संक्रियकः=(ध्रुव SmallString &other)
{
	यदि (एतत् != &other)
		store(other.view());
	प्रत्यागम *एतत्;
}

SmallString &SmallString::संक्रियकः=(SmallString &&other) noexcept
{
	यदि (एतत् == &other)
		प्रत्यागम *एतत्;
	release();
	स्मृतिं_प्रतिलिख(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	प्रत्यागम *एतत्;
}

SmallString::~SmallString() { release(); }

Twine_ग्रन्थिः::Twine_ग्रन्थिः(Twine_ग्रन्थिः &&other) noexcept
	: text_(std::move(other.text_)), उपसर्गः_3(other.उपसर्गः_3), hash_(other.hash_)
{
	other.उपसर्गः_3 = DEAD;
}

Twine_ग्रन्थिः &Twine_ग्रन्थिः::संक्रियकः=(Twine_ग्रन्थिः &&other) noexcept
{
	यदि (एतत् == &other)
		प्रत्यागम *एतत्;
	text_ = std::move(other.text_);
	उपसर्गः_3 = other.उपसर्गः_3;
	hash_ = other.hash_;
	other.उपसर्गः_3 = DEAD;
	प्रत्यागम *एतत्;
}

std::string TwineSpec::content_str() ध्रुव {
	यदि (स्वचालित *पर्णम्_2 = std::get_if<पर्णम्>(&दत्त))
		प्रत्यागम पर्णम्_2->s;
	स्वचालित &autosfx = std::get<AutoSuffix>(दत्त);
	प्रत्यागम *autosfx.उपसर्गः_2 + autosfx.पुच्छ;
}

स्थिर std::pair<std::string_view, तार्किक> unescape_name(std::string_view name) {
	यदि (!name.रिक्तानि() && name[0] == '\\')
		प्रत्यागम {name.substr(1), सत्य};
	प्रत्यागम {name, असत्य};
}

स्थिर शून्य check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	प्रति (वर्ण ch : name)
		यदि ((अचिह्नित)ch <= (अचिह्नित)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(अग्र_serial()) {}
TwinePool::TwinePool(ध्रुव TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(अग्र_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(अग्र_serial()) {}

TwinePool& TwinePool::संक्रियकः=(ध्रुव TwinePool& other) {
	यदि (एतत् == &other)
		प्रत्यागम *एतत्;
	HashConsPool::संक्रियकः=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = अग्र_serial();
	प्रत्यागम *एतत्;
}

TwinePool& TwinePool::संक्रियकः=(TwinePool&& other) {
	यदि (एतत् == &other)
		प्रत्यागम *एतत्;
	HashConsPool::संक्रियकः=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = अग्र_serial();
	प्रत्यागम *एतत्;
}

तार्किक TwinePool::owns(IdString ref) ध्रुव {
	प्रत्यागम ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ध्रुव {
	यदि (ref == IdString::Null || ID::is_static(ref))
		प्रत्यागम ref;
	प्रत्यागम ref.stamped(serial_);
}

शून्य TwinePool::check_owned(IdString ref) ध्रुव {
	यदि constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ध्रुव Twine_ग्रन्थिः& TwinePool::संक्रियकः[](IdString ref) ध्रुव {
	check_owned(ref);
	प्रत्यागम HashConsPool::संक्रियकः[](ref);
}

आकार_t TwinePool::gc(pool<IdString>& roots) {
	प्रति (स्वचालित &it : auto_prefixes)
		roots.निवेशय(it.second);
	प्रत्यागम HashConsPool::gc(roots);
}

ध्रुव Twine_ग्रन्थिः& TwinePool::static_ग्रन्थिः(आकार_t idx) { प्रत्यागम StaticTwines::ग्रन्थिः(idx); }
शून्य TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

आकार_t TwinePool::अग्र_serial() {
	स्थिर आकार_t गणक = 0;
	आकार_t serial = ++गणक;
	यदि constexpr (IdString::MAX_SERIAL == 0)
		प्रत्यागम serial;
	अन्यथा
		प्रत्यागम serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::सङ्कलय_inner(Twine_ग्रन्थिः t) {
	यदि (free_list.रिक्तानि() && STATIC_गणनम् + backing.आकार() > IdString::MAX_सूचकांक)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_सूचकांक - STATIC_गणनम्);
	प्रत्यागम HashConsPool::सङ्कलय_inner(std::move(t));
}

शून्य TwinePool::dump(IdString ref, std::ostream& os) ध्रुव {
	ध्रुव Twine_ग्रन्थिः& twine = (*एतत्)[ref];
	विकल्प (twine.विधा()) {
	पक्ष Twine_ग्रन्थिः::विधा_2::Dead:
		os << "Dead()";
		भङ्ग;
	पक्ष Twine_ग्रन्थिः::विधा_2::पर्णम्:
		os << "Leaf(\"" << twine.text() << "\")";
		भङ्ग;
	पक्ष Twine_ग्रन्थिः::विधा_2::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.उपसर्गः_2(), os);
		os << ", tail: \"" << twine.text() << "\")";
		भङ्ग;
	}
	यदि (ref.is_लौकिकम्())
		os << " pub";
}

शून्य TwinePool::append_str(IdString ref, std::string& बहिर्गमनम्) ध्रुव {
	यदि (ref == IdString::Null)
		प्रत्यागम;
	यदि (ref.is_लौकिकम्())
		बहिर्गमनम् += '\\';
	ध्रुव Twine_ग्रन्थिः& twine = (*एतत्)[ref];
	विकल्प (twine.विधा()) {
	पक्ष Twine_ग्रन्थिः::विधा_2::Dead:
		भङ्ग;
	पक्ष Twine_ग्रन्थिः::विधा_2::Suffix:
		append_str(twine.उपसर्गः_2(), बहिर्गमनम्);
		[[fallthrough]];
	पक्ष Twine_ग्रन्थिः::विधा_2::पर्णम्:
		बहिर्गमनम् += twine.text();
		भङ्ग;
	}
}

std::string TwinePool::str(IdString ref) ध्रुव {
	std::string बहिर्गमनम्;
	append_str(ref, बहिर्गमनम्);
	प्रत्यागम बहिर्गमनम्;
}

std::string TwinePool::unescaped_str(IdString ref) ध्रुव {
	प्रत्यागम str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t उपसर्गः_2, std::string_view text) ध्रुव {
	uint32_t hash = उपसर्गः_2 == Twine_ग्रन्थिः::NO_उपसर्गः ? 0 : (*एतत्)[IdString(उपसर्गः_2)].hash();
	प्रत्यागम Twine_ग्रन्थिः::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t उपसर्गः_2, std::string_view text) ध्रुव {
	प्रत्यागम find_content(उपसर्गः_2, text, content_hash(उपसर्गः_2, text));
}

IdString TwinePool::intern(uint32_t उपसर्गः_2, std::string_view text) {
	log_assert(!Multithreading::सक्रिय());
	uint32_t hash = content_hash(उपसर्गः_2, text);
	IdString ref = find_content(उपसर्गः_2, text, hash);
	यदि (ref != IdString::Null)
		प्रत्यागम ref;
	प्रत्यागम सङ्कलय_inner(Twine_ग्रन्थिः{उपसर्गः_2, text, hash});
}

IdString TwinePool::find(std::string_view name) ध्रुव {
	यदि (name.रिक्तानि())
		प्रत्यागम IdString::Null;
	check_name(name);
	स्वचालित [content, is_लौकिकम्_2] = unescape_name(name);
	प्रत्यागम stamp(find_content(Twine_ग्रन्थिः::NO_उपसर्गः, content).चिह्नकम्(is_लौकिकम्_2));
}

IdString TwinePool::find(TwineSpec t) ध्रुव {
	यदि (std::holds_alternative<TwineSpec::AutoSuffix>(t.दत्त))
		प्रत्यागम stamp(find_content(Twine_ग्रन्थिः::NO_उपसर्गः, t.content_str()));
	यदि (स्वचालित *पर्णम्_2 = std::get_if<TwineSpec::पर्णम्>(&t.दत्त))
		प्रत्यागम stamp(find_content(Twine_ग्रन्थिः::NO_उपसर्गः, पर्णम्_2->s));
	ध्रुव TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.दत्त);
	प्रत्यागम stamp(find_content((uint32_t)sfx.उपसर्गः_2.untag().raw(), sfx.पुच्छ).चिह्नकम्(sfx.उपसर्गः_2.is_लौकिकम्()));
}

IdString TwinePool::सङ्कलय(TwineSpec t) {
	यदि (स्वचालित *ap = std::get_if<TwineSpec::AutoSuffix>(&t.दत्त))
		प्रत्यागम सङ्कलय(auto_उपसर्गः(ap->उपसर्गः_2), ap->पुच्छ);
	यदि (स्वचालित *पर्णम्_2 = std::get_if<TwineSpec::पर्णम्>(&t.दत्त))
		प्रत्यागम stamp(intern(Twine_ग्रन्थिः::NO_उपसर्गः, पर्णम्_2->s));
	ध्रुव TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.दत्त);
	प्रत्यागम सङ्कलय(sfx.उपसर्गः_2, sfx.पुच्छ);
}

IdString TwinePool::auto_उपसर्गः(ध्रुव std::string *उपसर्गः_2) {
	स्वचालित it = auto_prefixes.find(उपसर्गः_2);
	यदि (it != auto_prefixes.अन्तः())
		प्रत्यागम it->second;
	IdString ref = intern(Twine_ग्रन्थिः::NO_उपसर्गः, *उपसर्गः_2);
	auto_prefixes[उपसर्गः_2] = ref;
	प्रत्यागम ref;
}

IdString TwinePool::सङ्कलय(IdString उपसर्गः_2, std::string_view पुच्छ) {
	प्रत्यागम stamp(intern((uint32_t)उपसर्गः_2.untag().raw(), पुच्छ).चिह्नकम्(उपसर्गः_2.is_लौकिकम्()));
}

IdString TwinePool::सङ्कलय(std::string_view s) {
	यदि (s.रिक्तानि())
		प्रत्यागम IdString::Null;
	check_name(s);
	स्वचालित [content, is_लौकिकम्_2] = unescape_name(s);
	प्रत्यागम stamp(intern(Twine_ग्रन्थिः::NO_उपसर्गः, content).चिह्नकम्(is_लौकिकम्_2));
}

IdString TwinePool::copy_from(ध्रुव TwinePool& src, IdString ref) {
	IdString प्राप्तम् = find_from(src, ref);
	यदि (प्राप्तम् != IdString::Null || ref == IdString::Null)
		प्रत्यागम प्राप्तम्;
	ध्रुव Twine_ग्रन्थिः& t = src[ref.untag()];
	यदि (t.is_dead())
		प्रत्यागम IdString::Null;
	uint32_t उपसर्गः_2 = Twine_ग्रन्थिः::NO_उपसर्गः;
	यदि (t.is_suffix())
		उपसर्गः_2 = (uint32_t)copy_from(src, t.उपसर्गः_2()).untag().raw();
	प्रत्यागम stamp(intern(उपसर्गः_2, t.text()).चिह्नकम्(ref.is_लौकिकम्()));
}

std::string TwinePool::ref_token(IdString ref) ध्रुव {
	प्रत्यागम "#" + std::गन्तव्यम्_string((uint64_t)serial_) + ":"
		+ std::गन्तव्यम्_string((uint64_t)stamp(ref).द्वयङ्काः());
}

IdString TwinePool::ref_from_token(std::string_view token) ध्रुव {
	यदि (token.आकार() < 2 || token[0] != '#')
		प्रत्यागम IdString::Null;
	आकार_t sep = token.find(':');
	यदि (sep == std::string_view::npos)
		प्रत्यागम IdString::Null;
	आकार_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	प्रति (पूर्णाङ्क i = 0; i < 2; i++) {
		यदि (parts[i].रिक्तानि())
			प्रत्यागम IdString::Null;
		प्रति (वर्ण c : parts[i]) {
			यदि (c < '0' || c > '9')
				प्रत्यागम IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	यदि (fields[0] != serial_)
		प्रत्यागम IdString::Null;
	IdString ref(fields[1]);
	यदि (ref == IdString::Null)
		प्रत्यागम IdString::Null;
	प्रत्यागम is_जीवितम्(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
संरचना TwineSegments {
	स्थिर constexpr आकार_t SEGMENT_INLINE_गभीरता = 8;
	TwineSegments(ध्रुव TwinePool &pool, IdString ref);
	TwineSegments(ध्रुव TwinePool &pool, IdString शीर्ष, std::string_view पुच्छ);
	std::string_view peek();
	शून्य advance(आकार_t n);
	आकार_t योग_आकार() ध्रुव;
गुप्त:
	शून्य प्रक्षिप(std::string_view seg);
	std::string_view *segs();
	ध्रुव std::string_view *segs() ध्रुव;
	std::string_view inline_segs[SEGMENT_INLINE_गभीरता];
	std::सदिशम्<std::string_view> spill;
	आकार_t गणनम् = 0;
	आकार_t pos = 0;
	आकार_t off = 0;
};

TwineSegments::TwineSegments(ध्रुव TwinePool &pool, IdString ref)
{
	यदि (ref == IdString::Null)
		प्रत्यागम;
	प्रति (IdString cur = ref.untag(); ;) {
		ध्रुव Twine_ग्रन्थिः &t = pool[cur];
		विकल्प (t.विधा()) {
		पक्ष Twine_ग्रन्थिः::विधा_2::Suffix:
			प्रक्षिप(t.text());
			cur = t.उपसर्गः_2();
			अनुवर्त;
		पक्ष Twine_ग्रन्थिः::विधा_2::पर्णम्:
			प्रक्षिप(t.text());
			भङ्ग;
		पक्ष Twine_ग्रन्थिः::विधा_2::Dead:
			भङ्ग;
		}
		भङ्ग;
	}
	यदि (ref.is_लौकिकम्())
		प्रक्षिप("\\");
	std::reverse(segs(), segs() + गणनम्);
}

TwineSegments::TwineSegments(ध्रुव TwinePool &pool, IdString शीर्ष, std::string_view पुच्छ)
	: TwineSegments(pool, शीर्ष)
{
	प्रक्षिप(पुच्छ);
}

std::string_view TwineSegments::peek()
{
	यावत् (pos < गणनम्) {
		std::string_view seg = segs()[pos];
		यदि (off < seg.आकार())
			प्रत्यागम seg.substr(off);
		pos++;
		off = 0;
	}
	प्रत्यागम {};
}

शून्य TwineSegments::advance(आकार_t n) { off += n; }

आकार_t TwineSegments::योग_आकार() ध्रुव
{
	आकार_t योग_2 = 0;
	प्रति (आकार_t i = 0; i < गणनम्; i++)
		योग_2 += segs()[i].आकार();
	प्रत्यागम योग_2;
}

शून्य TwineSegments::प्रक्षिप(std::string_view seg)
{
	यदि (spill.रिक्तानि() && गणनम् < SEGMENT_INLINE_गभीरता) {
		inline_segs[गणनम्++] = seg;
		प्रत्यागम;
	}
	यदि (spill.रिक्तानि())
		spill.नियोजय(inline_segs, inline_segs + गणनम्);
	spill.प्रक्षिप_पृष्ठभागः(seg);
	गणनम्++;
}

std::string_view *TwineSegments::segs() { प्रत्यागम spill.रिक्तानि() ? inline_segs : spill.दत्त(); }
ध्रुव std::string_view *TwineSegments::segs() ध्रुव { प्रत्यागम spill.रिक्तानि() ? inline_segs : spill.दत्त(); }

आकार_t TwinePool::str_आकार(IdString ref) ध्रुव
{
	प्रत्यागम TwineSegments(*एतत्, ref).योग_आकार();
}

तार्किक TwinePool::begins_with(IdString ref, std::string_view उपसर्गः_2) ध्रुव
{
	TwineSegments segs(*एतत्, ref);
	यावत् (!उपसर्गः_2.रिक्तानि()) {
		std::string_view seg = segs.peek();
		यदि (seg.रिक्तानि())
			प्रत्यागम असत्य;
		आकार_t n = std::min(seg.आकार(), उपसर्गः_2.आकार());
		यदि (std::memcmp(seg.दत्त(), उपसर्गः_2.दत्त(), n) != 0)
			प्रत्यागम असत्य;
		segs.advance(n);
		उपसर्गः_2.remove_उपसर्गः(n);
	}
	प्रत्यागम सत्य;
}

तार्किक TwinePool::name_equal(IdString ref, std::string_view name) ध्रुव
{
	प्रत्यागम str_आकार(ref) == name.आकार() && begins_with(ref, name);
}

स्थिर पूर्णाङ्क compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	यावत् (सत्य) {
		std::string_view x = sa.peek(), y = sb.peek();
		यदि (x.रिक्तानि() || y.रिक्तानि())
			प्रत्यागम x.रिक्तानि() ? (y.रिक्तानि() ? 0 : -1) : 1;
		आकार_t n = std::min(x.आकार(), y.आकार());
		यदि (पूर्णाङ्क diff = std::memcmp(x.दत्त(), y.दत्त(), n); diff != 0)
			प्रत्यागम diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t उपसर्गः_2, std::string_view text, uint32_t hash) ध्रुव {
	प्रत्यागम find_hashed(hash, [&](IdString ref) {
		ध्रुव Twine_ग्रन्थिः &t = (*एतत्)[ref];
		यदि (t == Twine_ग्रन्थिः::कुञ्चिका_2{उपसर्गः_2, text})
			प्रत्यागम सत्य;
		IdString शीर्ष = उपसर्गः_2 == Twine_ग्रन्थिः::NO_उपसर्गः ? IdString::Null : IdString(उपसर्गः_2);
		TwineSegments sa(*एतत्, ref), sb(*एतत्, शीर्ष, text);
		प्रत्यागम compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(ध्रुव TwinePool& src, IdString ref) ध्रुव {
	यदि (ref == IdString::Null || ID::is_static(ref))
		प्रत्यागम ref;
	IdString idx = ref.untag();
	ध्रुव Twine_ग्रन्थिः& t = src[idx];
	यदि (t.is_dead())
		प्रत्यागम IdString::Null;
	IdString प्राप्तम् = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*एतत्, cand), sb(src, idx);
		प्रत्यागम compare_segments(sa, sb) == 0;
	});
	प्रत्यागम stamp(प्राप्तम्.चिह्नकम्(ref.is_लौकिकम्()));
}

IdString TwinePool::उपसर्गः_सम्बन्धि(IdString ref) ध्रुव {
	ध्रुव Twine_ग्रन्थिः &t = (*एतत्)[ref];
	यदि (!t.is_suffix())
		प्रत्यागम IdString::Null;
	प्रत्यागम stamp(t.उपसर्गः_2().चिह्नकम्(ref.is_लौकिकम्()));
}

पूर्णाङ्क TwinePool::compare_by_name(IdString a, IdString b) ध्रुव
{
	यदि (a == b)
		प्रत्यागम 0;
	यदि (a == IdString::Null)
		प्रत्यागम -1;
	यदि (b == IdString::Null)
		प्रत्यागम 1;

	यदि (a.is_लौकिकम्() == b.is_लौकिकम्()) {
		ध्रुव Twine_ग्रन्थिः &ta = (*एतत्)[a.untag()];
		ध्रुव Twine_ग्रन्थिः &tb = (*एतत्)[b.untag()];
		यदि (ta.is_पर्णम्() && tb.is_पर्णम्())
			प्रत्यागम ta.text().compare(tb.text());
		यदि (ta.is_suffix() && tb.is_suffix() && ta.उपसर्गः_2() == tb.उपसर्गः_2())
			प्रत्यागम ta.text().compare(tb.text());
	}

	TwineSegments sa(*एतत्, a), sb(*एतत्, b);
	प्रत्यागम compare_segments(sa, sb);
}

YOSYS_NAMESPACE_अन्तः
